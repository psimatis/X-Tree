#include "Hyperrectangle.hpp"
#include "float.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Tests {

class HyperrectangleTest : public ::testing::Test {
 protected:
  HyperrectangleTest() {}
  virtual ~HyperrectangleTest() {}

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(HyperrectangleTest, ConstructorTest) {
  Hyperrectangle<3> hr;
  hr[0] = Interval(3.f, 6.f);
  hr[1] = Interval(-2.f, 5.7f);
  hr[2] = Interval(-12.3f, -4.f);
  EXPECT_FLOAT_EQ(hr[0].begin(), 3.f)
      << "The first interval of the hyperrectangle should begin at 3.";
  EXPECT_FLOAT_EQ(hr[0].end(), 6.f)
      << "The first interval of the hyperrectangle should end at 6.";
  EXPECT_FLOAT_EQ(hr[1].begin(), -2.f)
      << "The second interval of the hyperrectangle should begin at -2.";
  EXPECT_FLOAT_EQ(hr[1].end(), 5.7f)
      << "The second interval of the hyperrectangle should end at 5.7.";
  EXPECT_FLOAT_EQ(hr[2].begin(), -12.3f)
      << "The third interval of the hyperrectangle should begin at -12.3.";
  EXPECT_FLOAT_EQ(hr[2].end(), -4.f)
      << "The third interval of the hyperrectangle should end at -4.";
}

TEST_F(HyperrectangleTest, OperatorEqualsTest) {
  Hyperrectangle<3> hr1;
  hr1[0] = Interval(3.f, 6.f);
  hr1[1] = Interval(-2.f, 5.7f);
  hr1[2] = Interval(-12.3f, -4.f);
  Hyperrectangle<3> hr2 = hr1;

  for (int i = 0; i < 3; ++i) {
    EXPECT_FLOAT_EQ(hr1[i].begin(), hr2[i].begin())
        << "The beginning of interval " << i <<
        " of hr2 should be equal to the beginning of hr1.";
    EXPECT_FLOAT_EQ(hr1[i].end(), hr2[i].end())
        << "The end of interval " << i << " of hr2 should be equal to the end of hr1.";
  }
}

TEST_F(HyperrectangleTest, GetAreaTest) {
  Hyperrectangle<4> hr;
  hr[0] = Interval(-2.f, 6.f);
  hr[1] = Interval(5.f, 8.f);
  hr[2] = Interval(-4.f, 1.f);
  hr[3] = Interval(-10.f, -7.f);
  EXPECT_FLOAT_EQ(hr.getArea(), 360.f)
      << "The area of the hyperrectangle should equal 360.";
}

TEST_F(HyperrectangleTest, ResetTest) {
  Hyperrectangle<2> hr;
  hr[0] = Interval(-3.f, 7.f);
  hr[1] = Interval(0.04f, 0.12f);
  hr.reset();
  EXPECT_FLOAT_EQ(hr[0].begin(), FLT_MAX)
      << "The value for the reset beginning of interval 0 should equal FLT_MAX.";
  EXPECT_FLOAT_EQ(hr[0].end(), -FLT_MAX)
      << "The value for the reset end of interval 0 should equal -FLT_MAX.";
  EXPECT_FLOAT_EQ(hr[1].begin(), FLT_MAX)
      << "The value for the reset beginning of interval 1 should equal FLT_MAX.";
  EXPECT_FLOAT_EQ(hr[1].end(), -FLT_MAX)
      << "The value for the reset end of interval 1 should equal -FLT_MAX.";
}

TEST_F(HyperrectangleTest, AdjustTest) {
  Hyperrectangle<2> hr1;
  hr1[0] = Interval(1.f, 5.f);
  hr1[1] = Interval(1.f, 5.f);
  // Adjust lower limit
  Hyperrectangle<2> hr2;
  hr2[0] = Interval(0.f, 5.f);
  hr2[1] = Interval(0.f, 3.f);
  hr1.adjust(hr2);
  EXPECT_FLOAT_EQ(hr1[0].begin(), 0.f)
      << "The adjusted begin of interval 0 of hr1 should be 0.";
  EXPECT_FLOAT_EQ(hr1[0].end(), 5.f)
      << "The adjusted end of interval 0 of hr1 should remain 5.";
  EXPECT_FLOAT_EQ(hr1[1].begin(), 0.f)
      << "The adjusted begin of interval 1 of hr1 should be 0.";
  EXPECT_FLOAT_EQ(hr1[1].end(), 5.f)
      << "The adjusted end of interval 1 of hr1 should remain 5.";
  // Adjust upper limit
  hr2[0] = Interval(0.f, 7.f);
  hr2[1] = Interval(3.f, 8.5f);
  hr1.adjust(hr2);
  EXPECT_FLOAT_EQ(hr1[0].begin(), 0.f)
      << "The adjusted begin of interval 0 of hr1 should remain 0.";
  EXPECT_FLOAT_EQ(hr1[0].end(), 7.f)
      << "The adjusted end of interval 0 of hr1 should be 7.";
  EXPECT_FLOAT_EQ(hr1[1].begin(), 0.f)
      << "The adjusted begin of interval 1 of hr1 should remain 0.";
  EXPECT_FLOAT_EQ(hr1[1].end(), 8.5f)
      << "The adjusted end of interval 1 of hr1 should be 8.5.";
  // Adjust both limits
  hr2[0] = Interval(-5.5f, 10.5f);
  hr2[1] = Interval(-15.5f, 15.5f);
  hr1.adjust(hr2);
  EXPECT_FLOAT_EQ(hr1[0].begin(), -5.5f)
      << "The adjusted begin of interval 0 of hr1 should be -5.5.";
  EXPECT_FLOAT_EQ(hr1[0].end(), 10.5f)
      << "The adjusted end of interval 0 of hr1 should be 10.5.";
  EXPECT_FLOAT_EQ(hr1[1].begin(), -15.5f)
      << "The adjusted begin of interval 1 of hr1 should be -15.5.";
  EXPECT_FLOAT_EQ(hr1[1].end(), 15.5f)
      << "The adjusted end of interval 1 of hr1 should be 15.5.";
}

}  // namespace Tests
