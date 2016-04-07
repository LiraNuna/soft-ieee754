#include "test-utils.h"

FLOAT_TEST_GROUP(Infinity);

TYPED_TEST(Infinity, Equality) {
  TypeParam zero = 0;
  TypeParam inf = std::numeric_limits<TypeParam>::infinity();

  EXPECT_EQ(inf,  inf);
  EXPECT_NE(inf, -inf);

  EXPECT_GT(inf, zero);
  EXPECT_GT(inf, -inf);

  EXPECT_LT(-inf, zero);
  EXPECT_LT(-inf, inf);
}
