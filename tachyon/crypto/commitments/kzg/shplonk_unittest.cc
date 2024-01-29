#include "tachyon/crypto/commitments/kzg/shplonk.h"

#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "tachyon/base/containers/container_util.h"
#include "tachyon/base/json/json.h"
#include "tachyon/crypto/transcripts/simple_transcript.h"
#include "tachyon/math/elliptic_curves/bn/bn254/bn254.h"
#include "tachyon/math/elliptic_curves/bn/bn254/g1.h"
#include "tachyon/math/elliptic_curves/bn/bn254/g2.h"
#include "tachyon/math/polynomials/univariate/univariate_evaluation_domain_factory.h"

namespace tachyon {  // namespace base
namespace crypto {

template <typename Poly, typename Commitment>
struct OwnedPolynomialOpening {
  Poly poly;
  Commitment commitment;
  typename Poly::Point point;
  typename Poly::Field opening;
};

template <typename Poly, typename Commitment>
struct OwnedPolynomialOpenings {
  std::vector<OwnedPolynomialOpening<Poly, Commitment>> prover_openings;

  template <typename PCS>
  void Validate(const PCS& pcs) const {
    for (const OwnedPolynomialOpening<Poly, Commitment>& owned_opening :
         prover_openings) {
      CHECK(owned_opening.poly.Evaluate(owned_opening.point) ==
            owned_opening.opening);
      Commitment commitment;
      CHECK(pcs.Commit(owned_opening.poly, &commitment));
      CHECK(commitment == owned_opening.commitment);
    }
  }

  std::vector<PolynomialOpening<Poly>> CreateProverOpenings() const {
    return base::Map(
        prover_openings,
        [](const OwnedPolynomialOpening<Poly, Commitment>& owned_opening) {
          return PolynomialOpening<Poly>(
              base::ShallowRef<const Poly>(&owned_opening.poly),
              base::DeepRef<const typename Poly::Point>(&owned_opening.point),
              owned_opening.opening);
        });
  }

  std::vector<PolynomialOpening<Poly, Commitment>> CreateVerifyingOpenings()
      const {
    return base::Map(
        prover_openings,
        [](const OwnedPolynomialOpening<Poly, Commitment>& owned_opening) {
          return PolynomialOpening<Poly, Commitment>(
              base::ShallowRef<const Commitment>(&owned_opening.commitment),
              base::DeepRef<const typename Poly::Point>(&owned_opening.point),
              owned_opening.opening);
        });
  }
};

}  // namespace crypto

namespace base {

template <typename Poly, typename Commitment>
class RapidJsonValueConverter<
    crypto::OwnedPolynomialOpening<Poly, Commitment>> {
 public:
  template <typename Allocator>
  static rapidjson::Value From(
      const crypto::OwnedPolynomialOpening<Poly, Commitment>& value,
      Allocator& allocator) {
    rapidjson::Value object(rapidjson::kObjectType);
    AddJsonElement(object, "poly", value.poly, allocator);
    AddJsonElement(object, "point", value.point, allocator);
    AddJsonElement(object, "opening", value.opening, allocator);
    AddJsonElement(object, "commitment", value.commitment, allocator);
    return object;
  }

  static bool To(const rapidjson::Value& json_value, std::string_view key,
                 crypto::OwnedPolynomialOpening<Poly, Commitment>* value,
                 std::string* error) {
    Poly poly;
    Commitment commitment;
    typename Poly::Point point;
    typename Poly::Field opening;
    if (!ParseJsonElement(json_value, "poly", &poly, error)) return false;
    if (!ParseJsonElement(json_value, "point", &point, error)) return false;
    if (!ParseJsonElement(json_value, "opening", &opening, error)) return false;
    if (!ParseJsonElement(json_value, "commitment", &commitment, error))
      return false;
    value->poly = std::move(poly);
    value->commitment = std::move(commitment);
    value->point = std::move(point);
    value->opening = std::move(opening);
    return true;
  }
};

template <typename Poly, typename Commitment>
class RapidJsonValueConverter<
    crypto::OwnedPolynomialOpenings<Poly, Commitment>> {
 public:
  template <typename Allocator>
  static rapidjson::Value From(
      const crypto::OwnedPolynomialOpenings<Poly, Commitment>& value,
      Allocator& allocator) {
    rapidjson::Value object(rapidjson::kObjectType);
    AddJsonElement(object, "prover_openings", value.prover_openings, allocator);
    return object;
  }

  static bool To(const rapidjson::Value& json_value, std::string_view key,
                 crypto::OwnedPolynomialOpenings<Poly, Commitment>* value,
                 std::string* error) {
    std::vector<crypto::OwnedPolynomialOpening<Poly, Commitment>>
        prover_openings;
    if (!ParseJsonElement(json_value, "prover_openings", &prover_openings,
                          error))
      return false;
    value->prover_openings = std::move(prover_openings);
    return true;
  }
};

}  // namespace base

namespace crypto {
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

TEST_F(SHPlonkTest, ComplexCreateAndVerifyProof) {
  OwnedPolynomialOpenings<Poly, Commitment> owned_openings;
  std::string error;
  EXPECT_TRUE(LoadAndParseJson(
      base::FilePath(
          "tachyon/crypto/commitments/kzg/test/shplonk_test_data.json"),
      &owned_openings, &error));
  EXPECT_TRUE(error.empty());

  owned_openings.Validate(pcs_);

  SimpleTranscriptWriter<Commitment> writer((base::Uint8VectorBuffer()));
  std::vector<PolynomialOpening<Poly>> prover_openings =
      owned_openings.CreateProverOpenings();
  ASSERT_TRUE(pcs_.CreateOpeningProof(prover_openings, &writer));

  base::Buffer read_buf(writer.buffer().buffer(), writer.buffer().buffer_len());
  SimpleTranscriptReader<Commitment> reader(std::move(read_buf));
  std::vector<PolynomialOpening<Poly, Commitment>> verifier_openings =
      owned_openings.CreateVerifyingOpenings();

  EXPECT_TRUE((pcs_.VerifyOpeningProof(verifier_openings, &reader)));
}

}  // namespace crypto
}  // namespace tachyon
