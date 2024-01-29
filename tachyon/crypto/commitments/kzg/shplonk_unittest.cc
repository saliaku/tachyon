#include "tachyon/crypto/commitments/kzg/shplonk.h"

#include <memory>

#include "gtest/gtest.h"

#include "tachyon/crypto/transcripts/simple_transcript.h"
#include "tachyon/math/elliptic_curves/bn/bn254/bn254.h"
#include "tachyon/math/elliptic_curves/bn/bn254/g1.h"
#include "tachyon/math/elliptic_curves/bn/bn254/g2.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain_factory.h"

namespace tachyon::crypto {

namespace {

class SHPlonkTest : public testing::Test {
 public:
  constexpr static size_t K = 4;
  constexpr static size_t N = size_t{1} << K;
  constexpr static size_t kMaxDegree = N - 1;

  using PCS =
      SHPlonk<math::bn254::BN254Curve, kMaxDegree, math::bn254::G1AffinePoint>;
  using F = PCS::Field;
  using Poly = PCS::Poly;
  using Commitment = PCS::Commitment;
  using Point = Poly::Point;

  static void SetUpTestSuite() { math::bn254::BN254Curve::Init(); }

  void SetUp() override {
    KZG<math::bn254::G1AffinePoint, kMaxDegree, math::bn254::G1AffinePoint> kzg;
    pcs_ = PCS(std::move(kzg));
    ASSERT_TRUE(pcs_.UnsafeSetup(N, F(2)));
  }

 protected:
  PCS pcs_;
};

}  // namespace

TEST_F(SHPlonkTest, CreateAndVerifyProof) {
  using PolyRef = base::ShallowRef<const Poly>;
  using PointRef = base::DeepRef<const Point>;
  using CommitmentRef = base::ShallowRef<const Commitment>;

  std::vector<Poly> polys =
      base::CreateVector(5, []() { return Poly::Random(kMaxDegree); });
  std::vector<F> points = {F(1), F(2), F(3), F(4), F(5)};
  std::vector<Commitment> commitments =
      base::Map(polys, [this](const Poly& poly) {
        Commitment commitment;
        CHECK(pcs_.Commit(poly, &commitment));
        return commitment;
      });

  // clang-format off
  std::vector<PolynomialOpening<Poly>> prover_openings_;
  prover_openings_.reserve(13);
  // {P₀, [x₀, x₁, x₂]}
  prover_openings_.emplace_back(PolyRef(&polys[0]), PointRef(&points[0]), polys[0].Evaluate(points[0]));
  prover_openings_.emplace_back(PolyRef(&polys[0]), PointRef(&points[1]), polys[0].Evaluate(points[1]));
  prover_openings_.emplace_back(PolyRef(&polys[0]), PointRef(&points[2]), polys[0].Evaluate(points[2]));
  // (P₁, [x₀, x₁, x₂]}
  prover_openings_.emplace_back(PolyRef(&polys[1]), PointRef(&points[0]), polys[1].Evaluate(points[0]));
  prover_openings_.emplace_back(PolyRef(&polys[1]), PointRef(&points[1]), polys[1].Evaluate(points[1]));
  prover_openings_.emplace_back(PolyRef(&polys[1]), PointRef(&points[2]), polys[1].Evaluate(points[2]));
  // (P₂, [x₀, x₁, x₂]}
  prover_openings_.emplace_back(PolyRef(&polys[2]), PointRef(&points[0]), polys[2].Evaluate(points[0]));
  prover_openings_.emplace_back(PolyRef(&polys[2]), PointRef(&points[1]), polys[2].Evaluate(points[1]));
  prover_openings_.emplace_back(PolyRef(&polys[2]), PointRef(&points[2]), polys[2].Evaluate(points[2]));
  // (P₃, [x₂, x₃]}
  prover_openings_.emplace_back(PolyRef(&polys[3]), PointRef(&points[2]), polys[3].Evaluate(points[2]));
  prover_openings_.emplace_back(PolyRef(&polys[3]), PointRef(&points[3]), polys[3].Evaluate(points[3]));
  // (P₄, [x₃, x₄]}
  prover_openings_.emplace_back(PolyRef(&polys[4]), PointRef(&points[3]), polys[4].Evaluate(points[3]));
  prover_openings_.emplace_back(PolyRef(&polys[4]), PointRef(&points[4]), polys[4].Evaluate(points[4]));

  std::vector<PolynomialOpening<Poly, Commitment>> verifier_openings;
  verifier_openings.reserve(13);
  // {C₀, [x₀, x₁, x₂]}
  verifier_openings.emplace_back(CommitmentRef(&commitments[0]), PointRef(&points[0]), polys[0].Evaluate(points[0]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[0]), PointRef(&points[1]), polys[0].Evaluate(points[1]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[0]), PointRef(&points[2]), polys[0].Evaluate(points[2]));
  // (C₁, [x₀, x₁, x₂]}
  verifier_openings.emplace_back(CommitmentRef(&commitments[1]), PointRef(&points[0]), polys[1].Evaluate(points[0]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[1]), PointRef(&points[1]), polys[1].Evaluate(points[1]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[1]), PointRef(&points[2]), polys[1].Evaluate(points[2]));
  // (C₂, [x₀, x₁, x₂]}
  verifier_openings.emplace_back(CommitmentRef(&commitments[2]), PointRef(&points[0]), polys[2].Evaluate(points[0]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[2]), PointRef(&points[1]), polys[2].Evaluate(points[1]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[2]), PointRef(&points[2]), polys[2].Evaluate(points[2]));
  // (C₃, [x₂, x₃]}
  verifier_openings.emplace_back(CommitmentRef(&commitments[3]), PointRef(&points[2]), polys[3].Evaluate(points[2]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[3]), PointRef(&points[3]), polys[3].Evaluate(points[3]));
  // (C₄, [x₃, x₄]}
  verifier_openings.emplace_back(CommitmentRef(&commitments[4]), PointRef(&points[3]), polys[4].Evaluate(points[3]));
  verifier_openings.emplace_back(CommitmentRef(&commitments[4]), PointRef(&points[4]), polys[4].Evaluate(points[4]));
  // clang-format on

  SimpleTranscriptWriter<Commitment> writer((base::Uint8VectorBuffer()));
  ASSERT_TRUE(pcs_.CreateOpeningProof(prover_openings_, &writer));

  base::Buffer read_buf(writer.buffer().buffer(), writer.buffer().buffer_len());
  SimpleTranscriptReader<Commitment> reader(std::move(read_buf));
  EXPECT_TRUE((pcs_.VerifyOpeningProof(verifier_openings, &reader)));
}

}  // namespace tachyon::crypto
