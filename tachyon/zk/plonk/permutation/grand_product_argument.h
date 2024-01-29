#ifndef TACHYON_ZK_PLONK_PERMUTATION_GRAND_PRODUCT_ARGUMENT_H_
#define TACHYON_ZK_PLONK_PERMUTATION_GRAND_PRODUCT_ARGUMENT_H_

#include <utility>
#include <vector>

#include "gtest/gtest_prod.h"

#include "tachyon/base/parallelize.h"
#include "tachyon/zk/base/blinded_polynomial.h"
#include "tachyon/zk/base/entities/prover_base.h"

namespace tachyon::zk::plonk {

class GrandProductArgument {
 public:
  // If the number of rows is within than the supported size of polynomial
  // commitment scheme, you should use this version. See lookup argument for use
  // case.
  template <typename PCS, typename Callable, typename Poly = typename PCS::Poly>
  static BlindedPolynomial<Poly> Commit(ProverBase<PCS>* prover,
                                        Callable numerator_callback,
                                        Callable denominator_callback) {
    using Evals = typename PCS::Evals;

    size_t size = prover->pcs().N();
    RowIndex blinding_factors = prover->blinder().blinding_factors();
    Evals z = CreatePolynomial<Evals>(size, blinding_factors,
                                      std::move(numerator_callback),
                                      std::move(denominator_callback));
    CHECK(prover->blinder().Blind(z));
    return prover->CommitAndWriteToProofWithBlind(z);
  }

  // If the number of rows is out of the supported size of polynomial
  // commitment scheme, you should use this version. See permutation argument
  // for use case.
  // See
  // https://zcash.github.io/halo2/design/proving-system/permutation.html#spanning-a-large-number-of-columns
  template <typename PCS, typename Callable, typename F,
            typename Poly = typename PCS::Poly>
  static BlindedPolynomial<Poly> CommitExcessive(ProverBase<PCS>* prover,
                                                 Callable numerator_callback,
                                                 Callable denominator_callback,
                                                 size_t num_cols, F& last_z) {
    using Evals = typename PCS::Evals;

    size_t size = prover->pcs().N();
    RowIndex blinding_factors = prover->blinder().blinding_factors();
    Evals z = CreatePolynomialExcessive<Evals>(
        size, blinding_factors, num_cols, last_z, std::move(numerator_callback),
        std::move(denominator_callback));
    CHECK(prover->blinder().Blind(z));
    return prover->CommitAndWriteToProofWithBlind(z);
  }

 private:
  FRIEND_TEST(LookupArgumentRunnerTest, ComputePermutationProduct);

  template <typename Evals, typename Callable>
  static Evals CreatePolynomial(size_t size, RowIndex blinding_factors,
                                Callable numerator_callback,
                                Callable denominator_callback) {
    using F = typename Evals::Field;

    std::vector<F> grand_product(size, F::Zero());

    base::Parallelize(grand_product, std::move(denominator_callback));

    F::BatchInverseInPlace(grand_product);

    base::Parallelize(grand_product, std::move(numerator_callback));

    F last_z = F::One();
    return DoCreatePolynomial<Evals>(last_z, size, grand_product,
                                     blinding_factors);
  }

  template <typename Evals, typename F, typename Callable>
  static Evals CreatePolynomialExcessive(size_t size, RowIndex blinding_factors,
                                         size_t num_cols, F& last_z,
                                         Callable numerator_callback,
                                         Callable denominator_callback) {
    std::vector<F> grand_product(size, F::One());

    for (size_t i = 0; i < num_cols; ++i) {
      base::Parallelize(grand_product, denominator_callback(i));
    }

    F::BatchInverseInPlace(grand_product);

    for (size_t i = 0; i < num_cols; ++i) {
      base::Parallelize(grand_product, numerator_callback(i));
    }

    return DoCreatePolynomial<Evals>(last_z, size, grand_product,
                                     blinding_factors);
  }

  template <typename Evals, typename F>
  static Evals DoCreatePolynomial(F& last_z, size_t size,
                                  const std::vector<F>& grand_product,
                                  RowIndex blinding_factors) {
    std::vector<F> z;
    z.resize(size);
    z[0] = last_z;
    for (size_t i = 0; i < size - blinding_factors - 1; ++i) {
      z[i + 1] = z[i] * grand_product[i];
    }
    last_z = z[size - blinding_factors - 1];
    return Evals(std::move(z));
  }
};

}  // namespace tachyon::zk::plonk

#endif  // TACHYON_ZK_PLONK_PERMUTATION_GRAND_PRODUCT_ARGUMENT_H_
