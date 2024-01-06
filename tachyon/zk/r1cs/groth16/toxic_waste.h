// Use of this source code is governed by a Apache-2.0 style license that
// can be found in the LICENSE.lambdaworks.

#ifndef TACHYON_ZK_R1CS_GROTH16_TOXIC_WASTE_H_
#define TACHYON_ZK_R1CS_GROTH16_TOXIC_WASTE_H_

#include <utility>

namespace tachyon::zk::groth16 {

template <typename F>
class ToxicWaste {
 public:
  ToxicWaste(const F& tau, const F& alpha, const F& beta, const F& gamma,
             const F& delta)
      : tau_(tau), alpha_(alpha), beta_(beta), gamma_(gamma), delta_(delta) {}
  ToxicWaste(F&& tau, F&& alpha, F&& beta, F&& gamma, F&& delta)
      : tau_(std::move(tau)),
        alpha_(std::move(alpha)),
        beta_(std::move(beta)),
        gamma_(std::move(gamma)),
        delta_(std::move(delta)) {}

  static ToxicWaste Random() {
    return {F::Random(), F::Random(), F::Random(), F::Random(), F::Random()};
  }

  const F& tau() const { return tau_; }
  const F& alpha() const { return alpha_; }
  const F& beta() const { return beta_; }
  const F& gamma() const { return gamma_; }
  const F& delta() const { return delta_; }

 private:
  F tau_;
  F alpha_;
  F beta_;
  F gamma_;
  F delta_;
};

}  // namespace tachyon::zk::groth16

#endif  // TACHYON_ZK_R1CS_GROTH16_TOXIC_WASTE_H_
