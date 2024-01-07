// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE.lambdaworks.

#ifndef TACHYON_ZK_R1CS_GROTH16_PROOF_H_
#define TACHYON_ZK_R1CS_GROTH16_PROOF_H_

#include <utility>

namespace tachyon::zk::groth16 {

template <typename Curve>
class Proof {
 public:
  using G1Point = typename Curve::G1Curve::AffinePointTy;
  using G2Point = typename Curve::G2Curve::AffinePointTy;

  Proof(const G1Point& a, const G2Point& b, const G1Point& c)
      : a_(a), b_(b), c_(c) {}
  Proof(G1Point&& a, G2Point&& b, G1Point&& c)
      : a_(std::move(a)), b_(std::move(b)), c_(std::move(c)) {}

  const G1Point& a() const { return a_; }
  const G2Point& b() const { return b_; }
  const G1Point& c() const { return c_; }

 private:
  G1Point a_;
  G2Point b_;
  G1Point c_;
};

}  // namespace tachyon::zk::groth16

#endif  // TACHYON_ZK_R1CS_GROTH16_PROOF_H_
