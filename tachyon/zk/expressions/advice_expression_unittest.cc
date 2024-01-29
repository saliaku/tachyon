#include "tachyon/zk/expressions/advice_expression.h"

#include "gtest/gtest.h"

#include "tachyon/math/elliptic_curves/bn/bn254/fr.h"

namespace tachyon::zk {

using Fr = math::bn254::Fr;

class AdviceExpressionTest : public testing::Test {
 public:
  static void SetUpTestSuite() { Fr::Init(); }
};

TEST_F(AdviceExpressionTest, DegreeComplexity) {
  std::unique_ptr<AdviceExpression<Fr>> expr =
      AdviceExpression<Fr>::CreateForTesting(plonk::AdviceQuery(
          1, plonk::Rotation(1), plonk::AdviceColumnKey(1, plonk::Phase(0))));
  EXPECT_EQ(expr->Degree(), size_t{1});
  EXPECT_EQ(expr->Complexity(), uint64_t{1});
}

}  // namespace tachyon::zk
