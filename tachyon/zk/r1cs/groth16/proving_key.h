// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE.lambdaworks.

#ifndef TACHYON_ZK_R1CS_GROTH16_PROVING_KEY_H_
#define TACHYON_ZK_R1CS_GROTH16_PROVING_KEY_H_

#include <utility>
#include <vector>

#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/logging.h"
#include "tachyon/zk/r1cs/groth16/toxic_waste.h"
#include "tachyon/zk/r1cs/quadratic_arithmetic_program.h"

namespace tachyon::zk::groth16 {

template <typename Curve>
class ProvingKey {
 public:
  using G1Point = typename Curve::G1Curve::AffinePointTy;
  using G2Point = typename Curve::G2Curve::AffinePointTy;

  ProvingKey() = default;
  ProvingKey(const G1Point& alpha_g1, const G1Point& beta_g1,
             const G2Point& beta_g2, const G1Point& delta_g1,
             const G2Point& delta_g2, const std::vector<G1Point>& l_tau_g1,
             const std::vector<G1Point>& r_tau_g1,
             const std::vector<G2Point>& r_tau_g2,
             const std::vector<G1Point>& k_tau_g1,
             const std::vector<G1Point>& z_powers_of_tau_g1)
      : alpha_g1_(alpha_g1),
        beta_g1_(beta_g1),
        beta_g2_(beta_g2),
        delta_g1_(delta_g1),
        delta_g2_(delta_g2),
        l_tau_g1_(l_tau_g1),
        r_tau_g1_(r_tau_g1),
        r_tau_g2_(r_tau_g2),
        k_tau_g1_(k_tau_g1),
        z_powers_of_tau_g1_(z_powers_of_tau_g1) {}
  ProvingKey(G1Point&& alpha_g1, G1Point&& beta_g1, G2Point&& beta_g2,
             G1Point&& delta_g1, G2Point&& delta_g2,
             std::vector<G1Point>&& l_tau_g1, std::vector<G1Point>&& r_tau_g1,
             std::vector<G2Point>&& r_tau_g2, std::vector<G1Point>&& k_tau_g1,
             std::vector<G1Point>&& z_powers_of_tau_g1)
      : alpha_g1_(std::move(alpha_g1)),
        beta_g1_(std::move(beta_g1)),
        beta_g2_(std::move(beta_g2)),
        delta_g1_(std::move(delta_g1)),
        delta_g2_(std::move(delta_g2)),
        l_tau_g1_(std::move(l_tau_g1)),
        r_tau_g1_(std::move(r_tau_g1)),
        r_tau_g2_(std::move(r_tau_g2)),
        k_tau_g1_(std::move(k_tau_g1)),
        z_powers_of_tau_g1_(std::move(z_powers_of_tau_g1)) {}

  template <typename F, size_t MaxDegree>
  static ProvingKey Setup(const QuadraticArithmeticProgram<F, MaxDegree>& qap) {
    return Setup(qap, ToxicWaste::Random());
  }

  template <typename F, size_t MaxDegree>
  static ProvingKey Setup(const QuadraticArithmeticProgram<F, MaxDegree>& qap,
                          const ToxicWaste& toxic_waste) {
    G1Point g1 = G1Point::Generator();
    G2Point g2 = G2Point::Generator();

    std::vector<G1Point> g1_points;
    g1_points.resize(3);
    {
      F scalar_fields[] = {
          toxic_waste.alpha(),
          toxic_waste.beta(),
          toxic_waste.delta(),
      };
      CHECK(G1Point::BatchMapScalarFieldToPoint(g1, scalar_fields, &g1_points));
    }

    std::vector<G2Point> g2_points;
    g2_points.resize(2);
    {
      F scalar_fields[] = {
          toxic_waste.beta(),
          toxic_waste.delta(),
      };
      CHECK(G1Point::BatchMapScalarFieldToPoint(g2, scalar_fields, &g2_points));
    }

    std::vector<F> l_tau = base::Map(
        qap.l(),
        [&tau = toxic_waste.tau()](const Poly& p) { return p.Evaluate(tau); });

    std::vector<G1Point> l_tau_g1;
    l_tau_g1.resize(l_tau.size());
    CHECK(G1Point::BatchMapScalarFieldToPoint(g1, l_tau, &l_tau_g1));

    std::vector<F> r_tau = base::Map(
        qap.r(),
        [&tau = toxic_waste.tau()](const Poly& p) { return p.Evaluate(tau); });

    std::vector<G1Point> r_tau_g1;
    r_tau_g1.resize(r_tau.size());
    CHECK(G1Point::BatchMapScalarFieldToPoint(g1, r_tau, &r_tau_g1));

    std::vector<G2Point> r_tau_g2;
    r_tau_g2.resize(r_tau.size());
    CHECK(G2Point::BatchMapScalarFieldToPoint(g2, r_tau, &r_tau_g2));

    F delta_inv = toxic_waste.delta().Inverse();

    std::vector<F> k_tau;
    k_tau.reserve(qap.l().size() - qap.num_public_inputs());
    for (size_t i = qap.num_public_inputs(); i < qap.l().size(); ++i) {
      F unshifted = toxic_waste.beta() * qap.l()[i];
      unshfited += toxic_waste.alpha() * qap.r()[i];
      unshfited += qap.o()[i].Evaluate(toxic_waste.tau());
      k_tau.push_back(delta_inv * unshifted);
    }

    std::vector<G1Point> k_tau_g1;
    k_tau_g1.resize(k_tau.size());
    CHECK(G1Point::BatchMapScalarFieldToPoint(g1, k_tau, &k_tau_g1));

    std::vector<F> z_powers_of_tau = F::GetSuccessivePowers(
        qap.NumGates() * 2, toxic_waste.tau(),
        delta_inv * (toxic_waste.tau().pow(qap.NumGates()) - F::One()));
    std::vector<G1Point> z_powers_of_tau_g1;
    z_powers_of_tau_g1.resize(z_powers_of_tau.size());
    CHECK(G1Point::BatchMapScalarFieldToPoint(g1, z_powers_of_tau,
                                              &z_powers_of_tau_g1));

    return {std::move(g1_points[0]), std::move(g1_points[1]),
            std::move(g2_points[0]), std::move(g1_points[2]),
            std::move(g2_points[1]), std::move(l_tau_g1),
            std::move(r_tau_g1),     std::move(r_tau_g2),
            std::move(k_tau_g1),     std::move(z_powers_of_tau_g1)};
  }

  const G1Point& alpha_g1() const { return alpha_g1_; }
  const G1Point& beta_g1() const { return beta_g1_; }
  const G2Point& beta_g2() const { return beta_g2_; }
  const G1Point& delta_g1() const { return delta_g1_; }
  const G2Point& delta_g2() const { return delta_g2_; }
  const std::vector<G1Point>& l_tau_g1() const { return l_tau_g1_; }
  const std::vector<G1Point>& r_tau_g1() const { return r_tau_g1_; }
  const std::vector<G2Point>& r_tau_g2() const { return r_tau_g2_; }
  const std::vector<G1Point>& k_tau_g1() const { return k_tau_g1_; }
  const std::vector<G1Point>& z_powers_of_tau_g1() const {
    return z_powers_of_tau_g1_;
  }

 private:
  // [α]₁
  G1Point alpha_g1_;
  // [β]₁
  G1Point beta_g1_;
  // [β]₂
  G2Point beta_g2_;
  // [δ]₁
  G1Point delta_g1_;
  // [δ]₂
  G2Point delta_g2_;
  // [A₀(τ)]₁, [A₁(τ)]₁, ..., [Aₙ₋₁(τ)]₁
  std::vector<G1Point> l_tau_g1_;
  // [B₀(τ)]₁, [B₁(τ)]₁, ..., [Bₙ₋₁(τ)]₁
  std::vector<G1Point> r_tau_g1_;
  // [B₀(τ)]₂, [B₁(τ)]₂, ..., [Bₙ₋₁(τ)]₂
  std::vector<G2Point> r_tau_g2_;
  // [Kₖ₊₁(τ)]₁, [Kₖ₊₂(τ)]₁, ..., [Kₙ₋₁(τ)]₁
  // where Kᵢ(τ) = δ⁻¹ * (β * l(τ) + α * r(τ) + o(τ)) and |k| is the number of
  // public inputs.
  std::vector<G1Point> k_tau_g1_;
  // [δ⁻¹ * t(τ) * τ⁰]₁, [δ⁻¹ * t(τ) * τ¹]₁, ..., [δ⁻¹ * t(τ) * τ²ⁿ⁻¹]₁
  // where t(τ) = τⁿ - 1.
  std::vector<G1Point> z_powers_of_tau_g1_;
};

}  // namespace tachyon::zk::groth16

#endif  // TACHYON_ZK_R1CS_GROTH16_PROVING_KEY_H_
