#include <gtest/gtest.h>

#include "ieee754.hpp"

typedef IEEE754<3, 4 > float8;
typedef IEEE754<10, 5 > float16;
typedef IEEE754<23, 8 > float32;
typedef IEEE754<52, 11 > float64;

typedef testing::Types<float8, float16, float32, float64> FloatTypes;

#define FLOAT_TEST_GROUP(name) \
  TYPED_TEST_CASE(name, FloatTypes); \
  template <typename TypeParam> struct name : public testing::Test {}
