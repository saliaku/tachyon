#include "tachyon/zk/plonk/base/column_key.h"

#include "absl/hash/hash_testing.h"
#include "gtest/gtest.h"

namespace tachyon::zk::plonk {

template <typename ColumnKey>
class ColumnKeyTest : public testing::Test {};

using ColumnKeyTypes =
    testing::Types<FixedColumnKey, AdviceColumnKey, InstanceColumnKey>;
TYPED_TEST_SUITE(ColumnKeyTest, ColumnKeyTypes);

TYPED_TEST(ColumnKeyTest, AnyColumnKeyConstruction) {
  using ColumnKey = TypeParam;

  AnyColumnKey any(ColumnKey(1));
  EXPECT_EQ(any.type(), ColumnKey::kDefaultType);
  AnyColumnKey any2;
  EXPECT_EQ(any2.type(), ColumnType::kAny);
  any2 = ColumnKey(1);
  EXPECT_EQ(any2.type(), ColumnKey::kDefaultType);

  if constexpr (std::is_same_v<ColumnKey, AdviceColumnKey>) {
    AnyColumnKey any(ColumnKey(1, kSecondPhase));
    EXPECT_EQ(any.phase(), kSecondPhase);
    AnyColumnKey any2;
    EXPECT_EQ(any2.phase(), kFirstPhase);
    any2 = ColumnKey(1, kSecondPhase);
    EXPECT_EQ(any2.phase(), kSecondPhase);
  }
}

TYPED_TEST(ColumnKeyTest, NonAnyColumnKeyConstruction) {
  using ColumnKey = TypeParam;

  ColumnKey c(ColumnKey(1));
  EXPECT_EQ(c.type(), ColumnKey::kDefaultType);
  c = ColumnKey(1);
  EXPECT_EQ(c.type(), ColumnKey::kDefaultType);
  ColumnKey c2(AnyColumnKey(1));
  EXPECT_EQ(c2.type(), ColumnKey::kDefaultType);
  ColumnKey c3;
  EXPECT_EQ(c3.type(), ColumnKey::kDefaultType);
  c3 = AnyColumnKey(1);
  EXPECT_EQ(c3.type(), ColumnKey::kDefaultType);
}

TYPED_TEST(ColumnKeyTest, Hash) {
  using ColumnKey = TypeParam;
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(
      std::make_tuple(ColumnKey(), ColumnKey(1))));
}

TEST(ColumnKeyTest, Equality) {
  // When types and indices are equal.
  EXPECT_EQ(AnyColumnKey(), AnyColumnKey());
  EXPECT_EQ(InstanceColumnKey(), InstanceColumnKey());
  EXPECT_EQ(AdviceColumnKey(), AdviceColumnKey());
  EXPECT_EQ(FixedColumnKey(), FixedColumnKey());

  // When types are different.
  EXPECT_NE(AnyColumnKey(), InstanceColumnKey());
  EXPECT_NE(AnyColumnKey(), AdviceColumnKey());
  EXPECT_NE(AnyColumnKey(), FixedColumnKey());
  EXPECT_NE(InstanceColumnKey(), AdviceColumnKey());
  EXPECT_NE(InstanceColumnKey(), FixedColumnKey());
  EXPECT_NE(AdviceColumnKey(), FixedColumnKey());

  // When types are equal but indices (and/or phase) are different.
  EXPECT_NE(AnyColumnKey(), AnyColumnKey(1));
  EXPECT_NE(InstanceColumnKey(), InstanceColumnKey(1));
  EXPECT_NE(AdviceColumnKey(), AdviceColumnKey(0, kSecondPhase));
  EXPECT_NE(AdviceColumnKey(), AdviceColumnKey(1, kSecondPhase));
  EXPECT_NE(FixedColumnKey(), FixedColumnKey(1));
}

TEST(ColumnKeyTest, Order) {
  EXPECT_DEATH(CHECK_LT(AnyColumnKey(), FixedColumnKey()), "");
  EXPECT_DEATH(CHECK_LT(FixedColumnKey(), AnyColumnKey()), "");

  EXPECT_FALSE(InstanceColumnKey() < InstanceColumnKey());
  EXPECT_FALSE(InstanceColumnKey() < InstanceColumnKey(1));

  EXPECT_FALSE(AdviceColumnKey() < AdviceColumnKey());
  EXPECT_FALSE(AdviceColumnKey() < AdviceColumnKey(0, kFirstPhase));
  EXPECT_FALSE(AdviceColumnKey() < AdviceColumnKey(1, kFirstPhase));
  EXPECT_TRUE(AdviceColumnKey() < AdviceColumnKey(0, kSecondPhase));

  EXPECT_FALSE(FixedColumnKey() < FixedColumnKey());
  EXPECT_FALSE(FixedColumnKey() < FixedColumnKey(1));

  EXPECT_TRUE(InstanceColumnKey() < AdviceColumnKey());
  EXPECT_TRUE(InstanceColumnKey(1) < AdviceColumnKey());
  EXPECT_TRUE(InstanceColumnKey() < FixedColumnKey());
  EXPECT_TRUE(InstanceColumnKey(1) < FixedColumnKey());
  EXPECT_TRUE(AdviceColumnKey() < FixedColumnKey());
  EXPECT_TRUE(AdviceColumnKey(1, kSecondPhase) < FixedColumnKey());
}

TEST(ColumnKeyBaseTest, Hash) {
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(
      std::make_tuple(ColumnKeyBase(), ColumnKeyBase(ColumnType::kFixed, 0),
                      ColumnKeyBase(ColumnType::kFixed, 1),
                      ColumnKeyBase(ColumnType::kAdvice, 0),
                      ColumnKeyBase(ColumnType::kAdvice, 1),
                      ColumnKeyBase(ColumnType::kInstance, 0),
                      ColumnKeyBase(ColumnType::kInstance, 1))));
}

}  // namespace tachyon::zk::plonk
