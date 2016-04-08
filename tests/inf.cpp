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

TYPED_TEST(Infinity, Addition) {
  TypeParam zero = 0;
  TypeParam inf = std::numeric_limits<TypeParam>::infinity();

  EXPECT_EQ( inf + zero,  inf);
  EXPECT_EQ(-inf + zero, -inf);
  EXPECT_EQ(zero +  inf,  inf);
  EXPECT_EQ(zero + -inf, -inf);

  EXPECT_EQ( inf +  inf,  inf);
  EXPECT_EQ(-inf + -inf, -inf);

  EXPECT_EQ(std::isnan( inf + -inf), true);
  EXPECT_EQ(std::isnan(-inf +  inf), true);
}

TYPED_TEST(Infinity, Subtraction) {
  TypeParam zero = 0;
  TypeParam inf = std::numeric_limits<TypeParam>::infinity();

  EXPECT_EQ( inf - zero,  inf);
  EXPECT_EQ(-inf - zero, -inf);
  EXPECT_EQ(zero -  inf, -inf);
  EXPECT_EQ(zero - -inf,  inf);

  EXPECT_EQ( inf - -inf,  inf);
  EXPECT_EQ(-inf -  inf, -inf);

  EXPECT_EQ(std::isnan( inf -  inf), true);
  EXPECT_EQ(std::isnan(-inf - -inf), true);

}
