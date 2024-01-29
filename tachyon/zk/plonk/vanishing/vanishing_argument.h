// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#ifndef TACHYON_ZK_PLONK_VANISHING_VANISHING_ARGUMENT_H_
#define TACHYON_ZK_PLONK_VANISHING_VANISHING_ARGUMENT_H_

#include <stdint.h>

#include <memory>
#include <utility>
#include <vector>

#include "tachyon/base/containers/container_util.h"
#include "tachyon/zk/base/entities/prover_base.h"
#include "tachyon/zk/plonk/constraint_system/constraint_system.h"
#include "tachyon/zk/plonk/vanishing/circuit_polynomial_builder.h"
#include "tachyon/zk/plonk/vanishing/graph_evaluator.h"

namespace tachyon::zk {

template <typename PCS>
class ProvingKey;

template <typename F>
class VanishingArgument {
 public:
  VanishingArgument() = default;

  static VanishingArgument Create(
      const ConstraintSystem<F>& constraint_system) {
    VanishingArgument evaluator;

    std::vector<ValueSource> parts;
    for (const Gate<F>& gate : constraint_system.gates()) {
      std::vector<ValueSource> tmp = base::Map(
          gate.polys(),
          [&evaluator](const std::unique_ptr<Expression<F>>& expression) {
            return evaluator.custom_gates_.AddExpression(expression.get());
          });
      parts.insert(parts.end(), std::make_move_iterator(tmp.begin()),
                   std::make_move_iterator(tmp.end()));
    }
    evaluator.custom_gates_.AddCalculation(Calculation::Horner(
        ValueSource::PreviousValue(), std::move(parts), ValueSource::Y()));

    for (const LookupArgument<F>& lookup : constraint_system.lookups()) {
      GraphEvaluator<F> graph;

      auto compress =
          [&graph](
              const std::vector<std::unique_ptr<Expression<F>>>& expressions) {
            std::vector<ValueSource> parts = base::Map(
                expressions,
                [&graph](const std::unique_ptr<Expression<F>>& expression) {
                  return graph.AddExpression(expression.get());
                });
            return graph.AddCalculation(
                Calculation::Horner(ValueSource::ZeroConstant(),
                                    std::move(parts), ValueSource::Theta()));
          };

      // A_compressed(X) = θᵐ⁻¹A₀(X) + θᵐ⁻²A₁(X) + ... + θAₘ₋₂(X) + Aₘ₋₁(X)
      ValueSource compressed_input_coset = compress(lookup.input_expressions());
      // S_compressed(X) = θᵐ⁻¹S₀(X) + θᵐ⁻²S₁(X) + ... + θSₘ₋₂(X) + Sₘ₋₁(X)
      ValueSource compressed_table_coset = compress(lookup.table_expressions());

      // S_compressed(X) + γ
      ValueSource right = graph.AddCalculation(
          Calculation::Add(compressed_table_coset, ValueSource::Gamma()));
      // A_compressed(X) + β
      ValueSource left = graph.AddCalculation(
          Calculation::Add(compressed_input_coset, ValueSource::Beta()));
      // (A_compressed(X) + β) * (S_compressed(X) + γ)
      graph.AddCalculation(Calculation::Mul(left, right));

      evaluator.lookups_.push_back(std::move(graph));
    }

    return evaluator;
  }

  const GraphEvaluator<F>& custom_gates() const { return custom_gates_; }
  const std::vector<GraphEvaluator<F>> lookups() const { return lookups_; }

  template <typename PCS, typename Poly = typename PCS::Poly,
            typename ExtendedEvals = typename PCS::ExtendedEvals>
  ExtendedEvals BuildExtendedCircuitColumn(
      ProverBase<PCS>* prover, const ProvingKey<PCS>& proving_key,
      const F& beta, const F& gamma, const F& theta, const F& y, const F& zeta,
      absl::Span<const F> challenges,
      const std::vector<PermutationCommitted<Poly>>& committed_permutations,
      const std::vector<std::vector<LookupCommitted<Poly>>>&
          committed_lookups_vec,
      const std::vector<RefTable<Poly>>& poly_tables) const {
    RowIndex blinding_factors = prover->blinder().blinding_factors();
    size_t cs_degree =
        proving_key.verifying_key().constraint_system().ComputeDegree();

    CircuitPolynomialBuilder<PCS> builder =
        CircuitPolynomialBuilder<PCS>::Create(
            prover->domain(), prover->extended_domain(), prover->pcs().N(),
            blinding_factors, cs_degree, &beta, &gamma, &theta, &y, &zeta,
            challenges, &proving_key, &committed_permutations,
            &committed_lookups_vec, &poly_tables);

    return builder.BuildExtendedCircuitColumn(custom_gates_, lookups_);
  }

 private:
  GraphEvaluator<F> custom_gates_;
  std::vector<GraphEvaluator<F>> lookups_;
};

}  // namespace tachyon::zk

#endif  // TACHYON_ZK_PLONK_VANISHING_VANISHING_ARGUMENT_H_
