// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE.lambdaworks.

#ifndef TACHYON_ZK_R1CS_QUADRATIC_ARITHMETIC_PROGRAM_H_
#define TACHYON_ZK_R1CS_QUADRATIC_ARITHMETIC_PROGRAM_H_

#include "absl/numeric/bits.h"

#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/logging.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain.h"

namespace tachyon::zk {

template <typename F, size_t MaxDegree>
class QuadraticArithmeticProgram {
 public:
  using Domain = math::UnivariateEvaluationDomain<F, MaxDegree>;
  using Poly = typename Domain::DensePoly;
  using Evals = typename Domain::Evals;

  QuadraticArithmeticProgram(size_t num_public_inputs,
                             const std::vector<Poly>& l,
                             const std::vector<Poly>& r,
                             const std::vector<Poly>& o)
      : num_public_inputs_(num_public_inputs), l_(l), r_(r), o_(o) {}

  QuadraticArithmeticProgram(size_t num_public_inputs, std::vector<Poly>&& l,
                             std::vector<Poly>&& r, std::vector<Poly>&& o)
      : num_public_inputs_(num_public_inputs),
        l_(std::move(l)),
        r_(std::move(r)),
        o_(std::move(o)) {}

  const std::vector<Poly>& l() const { return l_; }
  const std::vector<Poly>& r() const { return r_; }
  const std::vector<Poly>& o() const { return o_; }

  size_t NumGates() const { return l_[0].NumElements(); }
  // NOTE(chokobole): It was supposed to be named to |num_public_inputs()|, but
  // for consistency, it's named as follows.
  size_t NumPublicInputs() const { return num_public_inputs_; }
  size_t NumPrivateInputs() const { return l_.size() - num_public_inputs_; }
  size_t NumTotalInputs() const { return l_.size(); }

  static QuadraticArithmeticProgram FromVariableMatrices(
      size_t num_public_inputs, const std::vector<Evals>& l,
      const std::vector<Evals>& r, const std::vector<Evals>& o) {
    size_t num_total_inputs = l.size();
    CHECK_EQ(num_total_inputs, r.size());
    CHECK_EQ(num_total_inputs, o.size());
    CHECK_GT(num_total_inputs, 0);
    CHECK_LE(num_public_inputs, num_total_inputs);

    size_t num_constraints = l[0].size();
    std::unique_ptr<Domain> domain = Domain::Create(num_constraints);
    size_t pad_zeroes = absl::bit_ceil(num_constraints) - num_constraints;
    if (pad_zeroes == 0) {
      return {
          num_public_inputs,
          BuildVariablePolynomial(domain.get(), l),
          BuildVariablePolynomial(domain.get(), r),
          BuildVariablePolynomial(domain.get(), o),
      };
    } else {
      return {
          num_public_inputs,
          BuildVariablePolynomial(domain.get(), ApplyPadding(l, pad_zeroes)),
          BuildVariablePolynomial(domain.get(), ApplyPadding(r, pad_zeroes)),
          BuildVariablePolynomial(domain.get(), ApplyPadding(o, pad_zeroes)),
      };
    }
  }

 private:
  static std::vector<Evals> ApplyPadding(const std::vector<Evals>& columns,
                                         size_t pad_zeroes) {
    return base::Map(columns, [](const Evals& column) {
      std::vector<F> padded_column = column.evaluations();
      padded_column.resize(padded_column.size() + pad_zeroes);
      return Evals(std::move(padded_column));
    });
  }

  static std::vector<Poly> BuildVariablePolynomial(
      const Domain* domain, const std::vector<Evals>& columns) {
    return base::Map(columns,
                     [](const Evals& evals) { return domain->IFFT(evals); });
  }

  size_t num_public_inputs_;
  std::vector<Poly> l_;
  std::vector<Poly> r_;
  std::vector<Poly> o_;
};

}  // namespace tachyon::zk

#endif  // TACHYON_ZK_R1CS_QUADRATIC_ARITHMETIC_PROGRAM_H_
