// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE.lambdaworks.

#ifndef TACHYON_ZK_R1CS_GROTH16_VERIFY_H_
#define TACHYON_ZK_R1CS_GROTH16_VERIFY_H_

#include "tachyon/math/elliptic_curves/msm/variable_base_msm.h"
#include "tachyon/math/elliptic_curves/pairing/pairing.h"
#include "tachyon/math/elliptic_curves/point_conversions.h"
#include "tachyon/zk/r1cs/groth16/proof.h"
#include "tachyon/zk/r1cs/groth16/verifying_key.h"

namespace tachyon::zk::groth16 {

template <typename Curve, typename Container>
bool Verify(const VerifyingKey<Curve>& vk, const Proof<Curve>& proof,
            const Container& public_inputs) {
  using G1Point = typename Curve::G1Curve::AffinePointTy;
  using G2Point = typename Curve::G2Curve::AffinePointTy;
  using Bucket = typename math::VariableBaseMSM<G1Point>::Bucket;

  // |γ⁻¹ * (β * l(τ) + α * r(τ) + o(τ))|₁
  math::VariableBaseMSM<G1Point> msm;
  Bucket ret;
  CHECK(msm.Run(vk.k_tau_g1(), public_inputs, &ret));
  G1Point k_tau_assigned_verifier_g1 = math::ConvertPoint<G1Point>(ret);

  G1Point lhs_g1[] = {proof.c()};
  G2Point lhs_g2[] = {vk.delta_g2()};
  G1Point lhs2_g1[] = {k_tau_assigned_verifier_g1};
  G2Point lhs2_g2[] = {vk.gamma_g2()};
  G1Point rhs_g1[] = {proof.a()};
  G2Point rhs_g2[] = {proof.b()};
  return math::Pairing<Curve>(lhs_g1, lhs_g2) * vk.alpha_g1_times_beta_g2() *
             math::Pairing<Curve>(lhs2_g1, lhs2_g2) ==
         math::Pairing<Curve>(proof.a(), proof.b());
}

}  // namespace tachyon::zk::groth16

#endif  // TACHYON_ZK_R1CS_GROTH16_VERIFY_H_
