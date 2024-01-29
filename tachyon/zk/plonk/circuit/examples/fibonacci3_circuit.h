#ifndef TACHYON_ZK_PLONK_CIRCUIT_EXAMPLES_FIBONACCI3_CIRCUIT_H_
#define TACHYON_ZK_PLONK_CIRCUIT_EXAMPLES_FIBONACCI3_CIRCUIT_H_

#include <memory>
#include <utility>

#include "tachyon/zk/plonk/circuit/assigned_cell.h"
#include "tachyon/zk/plonk/circuit/circuit.h"
#include "tachyon/zk/plonk/circuit/examples/is_zero_chip.h"
#include "tachyon/zk/plonk/circuit/virtual_cells.h"

namespace tachyon::zk {

template <typename F>
struct Fibonacci3Config {
  using Field = F;

  Fibonacci3Config(const Selector& selector, const AdviceColumnKey& a,
                   const AdviceColumnKey& b, const AdviceColumnKey& c,
                   IsZeroConfig<F>&& a_equals_b, const AdviceColumnKey& output)
      : selector(selector),
        a(a),
        b(b),
        c(c),
        a_equals_b(std::move(a_equals_b)),
        output(output) {}

  Fibonacci3Config Clone() const {
    return Fibonacci3Config(selector, a, b, c, a_equals_b.Clone(), output);
  }

  Selector selector;
  AdviceColumnKey a;
  AdviceColumnKey b;
  AdviceColumnKey c;
  IsZeroConfig<F> a_equals_b;
  AdviceColumnKey output;
};

template <typename F>
class Fibonacci3Chip {
 public:
  explicit Fibonacci3Chip(Fibonacci3Config<F>&& config)
      : config_(std::move(config)) {}

  static Fibonacci3Config<F> Configure(ConstraintSystem<F>& meta) {
    Selector selector = meta.CreateSimpleSelector();
    AdviceColumnKey a = meta.CreateAdviceColumn();
    AdviceColumnKey b = meta.CreateAdviceColumn();
    AdviceColumnKey c = meta.CreateAdviceColumn();
    AdviceColumnKey output = meta.CreateAdviceColumn();

    AdviceColumnKey is_zero_advice_column = meta.CreateAdviceColumn();
    IsZeroConfig<F> a_equals_b = IsZeroChip<F>::Configure(
        meta,
        [&selector](VirtualCells<F>& meta) {
          return meta.QuerySelector(selector);
        },
        [&a, &b](VirtualCells<F>& meta) {
          return std::move(meta.QueryAdvice(a, Rotation::Cur())) -
                 std::move(meta.QueryAdvice(b, Rotation::Cur()));
        },
        is_zero_advice_column);

    meta.CreateGate(
        "f(a, b, c) = if a == b {c} else {a - b}",
        [&selector, &a, &b, &c, &output, &a_equals_b](VirtualCells<F>& meta) {
          std::unique_ptr<Expression<F>> s = meta.QuerySelector(selector);
          std::unique_ptr<Expression<F>> a_expr =
              meta.QueryAdvice(a, Rotation::Cur());
          std::unique_ptr<Expression<F>> b_expr =
              meta.QueryAdvice(b, Rotation::Cur());
          std::unique_ptr<Expression<F>> c_expr =
              meta.QueryAdvice(c, Rotation::Cur());
          std::unique_ptr<Expression<F>> output_expr =
              meta.QueryAdvice(output, Rotation::Cur());

          std::vector<Constraint<F>> constraints;
          constraints.emplace_back(
              std::move(s->Clone()) *
              (std::move(a_equals_b.expr()) *
               (std::move(output_expr->Clone()) - std::move(c_expr))));
          constraints.emplace_back(
              std::move(s) *
              (std::move(ExpressionFactory<F>::Constant(F::One())) -
               a_equals_b.expr()) *
              (std::move(output_expr) -
               (std::move(a_expr) - std::move(b_expr))));
          return constraints;
        });

    return Fibonacci3Config<F>(selector, a, b, c, std::move(a_equals_b),
                               output);
  }

  AssignedCell<F> Assign(Layouter<F>* layouter, const F& a, const F& b,
                         const F& c) const {
    AssignedCell<F> ret;
    layouter->AssignRegion(
        "f(a, b, c) = if a == b {c} else {a - b}",
        [this, &ret, &a, &b, &c](Region<F>& region) {
          config_.selector.Enable(region, 0);

          region.AssignAdvice("a", config_.a, 0,
                              [&a]() { return Value<F>::Known(a); });
          region.AssignAdvice("b", config_.b, 0,
                              [&b]() { return Value<F>::Known(b); });
          region.AssignAdvice("c", config_.c, 0,
                              [&c]() { return Value<F>::Known(c); });

          IsZeroChip<F> is_zero_chip(std::move(config_.a_equals_b.Clone()));
          is_zero_chip.Assign(region, 0, Value<F>::Known(a - b));

          F output = a == b ? c : a - b;
          AssignedCell<F> output_cell = region.AssignAdvice(
              "output", config_.output, 0,
              [&output]() { return Value<F>::Known(output); });

          ret = output_cell;
        });
    return ret;
  }

 private:
  Fibonacci3Config<F> config_;
};

template <typename F, template <typename> class _FloorPlanner>
class Fibonacci3Circuit : public Circuit<Fibonacci3Config<F>> {
 public:
  using FloorPlanner = _FloorPlanner<Fibonacci3Circuit<F, _FloorPlanner>>;

  Fibonacci3Circuit(const F& a, const F& b, const F& c) : a_(a), b_(b), c_(c) {}
  Fibonacci3Circuit(F&& a, F&& b, F&& c)
      : a_(std::move(a)), b_(std::move(b)), c_(std::move(c)) {}

  std::unique_ptr<Circuit<Fibonacci3Config<F>>> WithoutWitness()
      const override {
    return std::make_unique<Fibonacci3Circuit>(a_, b_, c_);
  }

  static Fibonacci3Config<F> Configure(ConstraintSystem<F>& meta) {
    return Fibonacci3Chip<F>::Configure(meta);
  }

  void Synthesize(Fibonacci3Config<F>&& config,
                  Layouter<F>* layouter) const override {
    Fibonacci3Chip<F> fibonacci3_chip(std::move(config));
    fibonacci3_chip.Assign(layouter, a_, b_, c_);
  }

 private:
  F a_;
  F b_;
  F c_;
};

}  // namespace tachyon::zk

#endif  // TACHYON_ZK_PLONK_CIRCUIT_EXAMPLES_FIBONACCI3_CIRCUIT_H_
