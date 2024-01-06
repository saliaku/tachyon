// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE.lambdaworks.

#ifndef TACHYON_ZK_R1CS_GROTH16_VERIFYING_KEY_H_
#define TACHYON_ZK_R1CS_GROTH16_VERIFYING_KEY_H_

#include <utility>
#include <vector>

#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/logging.h"
#include "tachyon/math/elliptic_curves/pairing/pairing.h"
#include "tachyon/zk/r1cs/groth16/toxic_waste.h"
#include "tachyon/zk/r1cs/quadratic_arithmetic_program.h"

namespace tachyon::zk::groth16 {

template <typename Curve>
class VerifyingKey {
 public:
  using G1Point = typename Curve::G1Curve::AffinePointTy;
  using G2Point = typename Curve::G2Curve::AffinePointTy;
  using G2Prepared = typename Curve::G2Prepared;
  using Fp12 = typename Curve::Fp12;

  VerifyingKey() = default;
  VerifyingKey(const Fp12& alpha_g1_times_beta_g2, const G2Point& delta_g2,
               const G2Point& gamma_g2, const std::vector<G1Point>& k_tau_g1)
      : alpha_g1_times_beta_g2_(alpha_g1_times_beta_g2),
        delta_g2_(delta_g2),
        gamma_g2_(gamma_g2),
        k_tau_g1_(k_tau_g1) {}
  VerifyingKey(Fp12&& alpha_g1_times_beta_g2, G2Point&& delta_g2,
               G2Point&& gamma_g2, std::vector<G1Point>&& k_tau_g1)
      : alpha_g1_times_beta_g2_(std::move(alpha_g1_times_beta_g2)),
        delta_g2_(std::move(delta_g2)),
        gamma_g2_(std::move(gamma_g2)),
        k_tau_g1_(std::move(k_tau_g1)) {}

  template <typename F, size_t MaxDegree>
  static VerifyingKey Setup(
      const QuadraticArithmeticProgram<F, MaxDegree>& qap) {
    return Setup(qap, ToxicWaste::Random());
  }

  template <typename F, size_t MaxDegree>
  static VerifyingKey Setup(const QuadraticArithmeticProgram<F, MaxDegree>& qap,
                            const ToxicWaste& toxic_waste) {
    G1Point g1 = G1Point::Generator();
    G2Point g2 = G2Point::Generator();

    Fp12 alpha_g1_times_beta_g2 =
        Pairing::compute(&alpha_g1, &beta_g2).unwrap();

    std::vector<G2Point> g2_points;
    g2_points.resize(3);
    {
      F scalar_fields[] = {
          toxic_waste.beta(),
          toxic_waste.delta(),
          toxic_waste.gamma(),
      };
      CHECK(G1Point::BatchMapScalarFieldToPoint(g2, scalar_fields, &g2_points));
    }

    G1PointTy alpha_g1[] = {(toxic_waste.alpha() * g1).ToAffine()};
    G2Prepared beta_g2[] = {G2Prepared::From(g2_points[0])};

    std::vector<F> k_tau;
    k_tau.reserve(qap.num_public_inputs());
    for (size_t i = 0; i < qap.num_public_inputs(); ++i) {
      F unshifted = toxic_waste.beta() * qap.l()[i];
      unshfited += toxic_waste.alpha() * qap.r()[i];
      unshfited += qap.o()[i].Evaluate(tau);
      k_tau.push_back(gamma_inv * unshifted);
    }

    std::vector<G1Point> k_tau_g1;
    k_tau_g1.resize(k_tau.size());
    CHECK(G1Point::BatchMapScalarFieldToPoint(g1, k_tau, &k_tau_g1));

    return {math::Pairing<CurveTy>(alpha_g1, beta_g2), std::move(g2_points[1]),
            std::move(g2_points[2]), std::move(k_tau_g1)};
  }

  const Fp12& alpha_g1_times_beta_g2() const { return alpha_g1_times_beta_g2_; }
  const G2Point& delta_g2() const { return delta_g2_; }
  const G2Point& gamma_g2() const { return gamma_g2_; }
  const std::vector<G1Point>& k_tau_g1() const { return k_tau_g1_; }

 private:
  // e([α]₁, [β]₂)
  Fp12 alpha_g1_times_beta_g2_;
  // [δ]₂
  G2Point delta_g2_;
  // [γ]₂
  G2Point gamma_g2_;
  // [K₀(τ)]₁, [K₁(τ)]₁, ..., [Kₖ₋₁(τ)]₁
  // where Kᵢ(τ) = γ⁻¹ * (β * l(τ) + α * r(τ) + o(τ)) and |k| is the number of
  // public inputs.
  std::vector<G1Point> k_tau_g1_;
};

}  // namespace tachyon::zk::groth16

#endif  // TACHYON_ZK_R1CS_GROTH16_VERIFYING_KEY_H_
