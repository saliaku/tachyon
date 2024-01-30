#include "vendors/halo2/include/bn254_shplonk_prover.h"

#include "tachyon/base/buffer/buffer.h"
#include "tachyon/c/zk/plonk/keys/proving_key_impl_base.h"
#include "tachyon/math/elliptic_curves/msm/variable_base_msm.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain_factory.h"
#include "tachyon/rs/base/container_util.h"
#include "tachyon/rs/base/rust_vec_copyable.h"
#include "vendors/halo2/include/bn254_evals.h"
#include "vendors/halo2/include/bn254_poly.h"
#include "vendors/halo2/include/bn254_rational_evals.h"
#include "vendors/halo2/src/bn254.rs.h"
#include "vendors/halo2/src/bn254_evals_impl.h"
#include "vendors/halo2/src/bn254_poly_impl.h"
#include "vendors/halo2/src/bn254_proving_key_impl.h"
#include "vendors/halo2/src/bn254_rational_evals_impl.h"
#include "vendors/halo2/src/bn254_shplonk_prover_impl.h"

namespace tachyon::halo2_api::bn254 {

namespace {

rust::Box<G1JacobianPoint> DoCommit(
    const std::vector<math::bn254::G1AffinePoint>& cpp_bases,
    const std::vector<math::bn254::Fr>& scalars) {
  math::VariableBaseMSM<math::bn254::G1AffinePoint> msm;
  math::VariableBaseMSM<math::bn254::G1AffinePoint>::Bucket bucket;
  CHECK(msm.Run(cpp_bases, scalars, &bucket));
  math::bn254::G1JacobianPoint* result = new math::bn254::G1JacobianPoint;
  *result = bucket.ToJacobian();
  return rust::Box<G1JacobianPoint>::from_raw(
      reinterpret_cast<G1JacobianPoint*>(result));
}

PCS::Evals ReadEvals(uint8_t* vec_ptr, size_t i) {
  size_t num_bytes = sizeof(uintptr_t);
  base::Buffer buffer(&vec_ptr[num_bytes * i], num_bytes);
  uintptr_t ptr;
  CHECK(buffer.Read(&ptr));
  Evals* evals = reinterpret_cast<Evals*>(ptr);
  return std::move(*evals->impl()).TakeEvals();
}

PCS::Poly ReadPoly(uint8_t* vec_ptr, size_t i) {
  size_t num_bytes = sizeof(uintptr_t);
  base::Buffer buffer(&vec_ptr[num_bytes * i], num_bytes);
  uintptr_t ptr;
  CHECK(buffer.Read(&ptr));
  Poly* poly = reinterpret_cast<Poly*>(ptr);
  return std::move(*poly->impl()).TakePoly();
}

}  // namespace

SHPlonkProver::SHPlonkProver(uint32_t k, const Fr& s)
    : impl_(new SHPlonkProverImpl(k, s)) {}

uint32_t SHPlonkProver::k() const { return static_cast<uint32_t>(impl_->K()); }

uint64_t SHPlonkProver::n() const { return static_cast<uint64_t>(impl_->N()); }

rust::Box<G1JacobianPoint> SHPlonkProver::commit(const Poly& poly) const {
  return DoCommit(impl_->prover().pcs().GetG1PowersOfTau(),
                  poly.impl()->poly().coefficients().coefficients());
}

rust::Box<G1JacobianPoint> SHPlonkProver::commit_lagrange(
    const Evals& evals) const {
  return DoCommit(impl_->prover().pcs().GetG1PowersOfTauLagrange(),
                  evals.impl()->evals().evaluations());
}

std::unique_ptr<Evals> SHPlonkProver::empty_evals() const {
  const PCS::Domain* domain = impl_->prover().domain();
  PCS::Evals evals = domain->Empty<PCS::Evals>();
  std::unique_ptr<Evals> ret(new Evals());
  PCS::Evals& impl = reinterpret_cast<PCS::Evals&>(ret->impl()->evals());
  impl = std::move(evals);
  return ret;
}

std::unique_ptr<RationalEvals> SHPlonkProver::empty_rational_evals() const {
  const PCS::Domain* domain = impl_->prover().domain();
  PCS::RationalEvals evals = domain->Empty<PCS::RationalEvals>();
  std::unique_ptr<RationalEvals> ret(new RationalEvals());
  PCS::RationalEvals& impl =
      reinterpret_cast<PCS::RationalEvals&>(ret->impl()->evals());
  impl = std::move(evals);
  return ret;
}

std::unique_ptr<Poly> SHPlonkProver::ifft(const Evals& evals) const {
  const PCS::Domain* domain = impl_->prover().domain();
  PCS::Poly poly =
      domain->IFFT(reinterpret_cast<const PCS::Evals&>(evals.impl()->evals()));
  std::unique_ptr<Poly> ret(new Poly());
  PCS::Poly& impl = reinterpret_cast<PCS::Poly&>(ret->impl()->poly());
  impl = std::move(poly);
  // NOTE(chokobole): The zero degrees might be removed. This is not compatible
  // with rust halo2.
  impl.coefficients().coefficients().resize(domain->size());
  return ret;
}

void SHPlonkProver::batch_evaluate(
    rust::Slice<std::unique_ptr<RationalEvals>> rational_evals,
    rust::Slice<std::unique_ptr<Evals>> evals) const {
  std::vector<PCS::RationalEvals> cpp_rational_evals = base::Map(
      rational_evals, [](std::unique_ptr<RationalEvals>& rational_eval) {
        return PCS::RationalEvals(
            std::move(*rational_eval->impl()).TakeEvals());
      });
  for (size_t i = 0; i < cpp_rational_evals.size(); ++i) {
    const PCS::RationalEvals& cpp_rational_eval = cpp_rational_evals[i];
    std::vector<math::bn254::Fr> cpp_values(cpp_rational_eval.NumElements());
    CHECK(math::RationalField<math::bn254::Fr>::BatchEvaluate(
        cpp_rational_eval.evaluations(), &cpp_values));
    evals[i]->impl()->evals() = PCS::Evals(std::move(cpp_values));
  }
}

void SHPlonkProver::set_rng(rust::Slice<const uint8_t> state) {
  base::Buffer buffer(const_cast<uint8_t*>(state.data()), state.size());
  uint32_t x, y, z, w;
  CHECK(buffer.Read32LE(&x));
  CHECK(buffer.Read32LE(&y));
  CHECK(buffer.Read32LE(&z));
  CHECK(buffer.Read32LE(&w));
  impl_->SetRng(std::make_unique<crypto::XORShiftRNG>(
      crypto::XORShiftRNG::FromState(x, y, z, w)));
}

void SHPlonkProver::set_transcript(rust::Slice<const uint8_t> state) {
  base::Uint8VectorBuffer write_buf;
  std::unique_ptr<zk::halo2::Blake2bWriter<math::bn254::G1AffinePoint>> writer =
      std::make_unique<zk::halo2::Blake2bWriter<math::bn254::G1AffinePoint>>(
          std::move(write_buf));
  writer->SetState(rs::ConvertRustSliceToCppSpan<const uint8_t>(state));
  impl_->SetTranscript(std::move(writer));
}

void SHPlonkProver::set_extended_domain(const ProvingKey& pk) {
  const c::zk::ProvingKeyImplBase<PCS::Poly, PCS::Evals, PCS::Commitment>*
      cpp_key = reinterpret_cast<const c::zk::ProvingKeyImplBase<
          PCS::Poly, PCS::Evals, PCS::Commitment>*>(pk.impl()->pk());
  impl_->SetExtendedDomain(cpp_key->GetConstraintSystem());
}

void SHPlonkProver::create_proof(const ProvingKey& key,
                                 rust::Vec<InstanceSingle> instance_singles,
                                 rust::Vec<AdviceSingle> advice_singles,
                                 rust::Vec<Fr> challenges) {
  const c::zk::ProvingKeyImplBase<PCS::Poly, PCS::Evals, PCS::Commitment>*
      cpp_key = reinterpret_cast<const c::zk::ProvingKeyImplBase<
          PCS::Poly, PCS::Evals, PCS::Commitment>*>(key.impl()->pk());
  impl_->SetBlindingFactors(
      cpp_key->verifying_key().constraint_system().ComputeBlindingFactors());

  size_t num_circuits = instance_singles.size();
  CHECK_EQ(num_circuits, advice_singles.size())
      << "size of |instance_singles| and |advice_singles| don't match";

  std::vector<std::vector<PCS::Evals>> advice_columns_vec;
  advice_columns_vec.resize(num_circuits);
  std::vector<std::vector<math::bn254::Fr>> advice_blinds_vec;
  advice_blinds_vec.resize(num_circuits);

  std::vector<math::bn254::Fr> cpp_challenges =
      base::Map(challenges, [](const Fr& fr) {
        return reinterpret_cast<const math::bn254::Fr&>(fr);
      });

  std::vector<std::vector<PCS::Evals>> instance_columns_vec;
  instance_columns_vec.resize(num_circuits);
  std::vector<std::vector<PCS::Poly>> instance_polys_vec;
  instance_polys_vec.resize(num_circuits);

  // TODO(chokobole): We shouldn't copy values here in the next iteration.
  size_t num_bytes = base::EstimateSize(rs::RustVec());
  for (size_t i = 0; i < num_circuits; ++i) {
    uint8_t* buffer_ptr = reinterpret_cast<uint8_t*>(advice_singles.data());
    base::Buffer buffer(&buffer_ptr[num_bytes * 2 * i], num_bytes * 2);
    rs::RustVec vec;

    CHECK(buffer.Read(&vec));
    size_t num_advice_columns = vec.length;
    uint8_t* vec_ptr = reinterpret_cast<uint8_t*>(vec.ptr);
    advice_columns_vec[i] = base::CreateVector(
        num_advice_columns,
        [vec_ptr](size_t j) { return ReadEvals(vec_ptr, j); });

    CHECK(buffer.Read(&vec));
    vec_ptr = reinterpret_cast<uint8_t*>(vec.ptr);
    advice_blinds_vec[i] = vec.ToVec<math::bn254::Fr>();

    buffer_ptr = reinterpret_cast<uint8_t*>(instance_singles.data());
    buffer = base::Buffer(&buffer_ptr[num_bytes * 2 * i], num_bytes * 2);

    CHECK(buffer.Read(&vec));
    size_t num_instance_columns = vec.length;
    vec_ptr = reinterpret_cast<uint8_t*>(vec.ptr);
    instance_columns_vec[i] = base::CreateVector(
        num_instance_columns,
        [vec_ptr](size_t j) { return ReadEvals(vec_ptr, j); });

    CHECK(buffer.Read(&vec));
    CHECK_EQ(num_instance_columns, vec.length)
        << "size of instance columns don't match";
    vec_ptr = reinterpret_cast<uint8_t*>(vec.ptr);
    instance_polys_vec[i] = base::CreateVector(
        num_instance_columns,
        [vec_ptr](size_t j) { return ReadPoly(vec_ptr, j); });
  }

  zk::halo2::ArgumentData<PCS::Poly, PCS::Evals> argument_data(
      std::move(advice_columns_vec), std::move(advice_blinds_vec),
      std::move(cpp_challenges), std::move(instance_columns_vec),
      std::move(instance_polys_vec));
  impl_->CreateProof(*cpp_key, &argument_data);
}

rust::Vec<uint8_t> SHPlonkProver::finalize_transcript() {
  return rs::ConvertCppContainerToRustVec(impl_->GetTranscriptOwnedBuffer());
}

std::unique_ptr<SHPlonkProver> new_shplonk_prover(uint32_t k, const Fr& s) {
  return std::make_unique<SHPlonkProver>(k, s);
}

rust::Box<Fr> ProvingKey::transcript_repr(const SHPlonkProver& prover) {
  c::zk::ProvingKeyImplBase<PCS::Poly, PCS::Evals, PCS::Commitment>* cpp_key =
      reinterpret_cast<
          c::zk::ProvingKeyImplBase<PCS::Poly, PCS::Evals, PCS::Commitment>*>(
          impl_->pk());
  cpp_key->SetTranscriptRepr(prover.impl()->prover());
  math::bn254::Fr* ret =
      new math::bn254::Fr(cpp_key->GetTranscriptRepr(prover.impl()->prover()));
  return rust::Box<Fr>::from_raw(reinterpret_cast<Fr*>(ret));
}

}  // namespace tachyon::halo2_api::bn254
