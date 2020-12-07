#include "Interval.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Tests {

class IntervalTest : public ::testing::Test {
 protected:
  IntervalTest() {}
  virtual ~IntervalTest() {}

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(IntervalTest, ConstructorTest) {
  {
    Interval i{3.f, 4.f};
    EXPECT_FLOAT_EQ(i.begin(), 3.f)
        << "The beginning of the interval should be 3, as initialized.";
    EXPECT_FLOAT_EQ(i.end(), 4.f)
        << "The end of the interval should be 4, as initialized.";
  }
  {
    Interval i{-5.8f, 6.9f};
    EXPECT_FLOAT_EQ(i.begin(), -5.8f)
        << "The beginning of the interval should be -5.8, as initialized.";
    EXPECT_FLOAT_EQ(i.end(), 6.9f)
        << "The end of the interval should be 6.9, as initialized.";
  }
  {
    Interval i{-9.7f, -3.2f};
    EXPECT_FLOAT_EQ(i.begin(), -9.7f)
        << "The beginning of the interval should be -9.7, as initialized.";
    EXPECT_FLOAT_EQ(i.end(), -3.2f)
        << "The end of the interval should be -3.2, as initialized.";
  }
  std::string error_message;

  try {
    Interval i{9.f, 6.3f};
  } catch (const std::invalid_argument& e) {
    error_message = e.what();
  }

  EXPECT_EQ(error_message, "Lower limit of an interval cannot be greater"
            " than the upper limit.")
      << "Lower limit > Upper limit didn't raise an error [9, 6.3].";

  try {
    Interval i{10.f, -5.f};
  } catch (const std::invalid_argument& e) {
    error_message = e.what();
  }

  EXPECT_EQ(error_message, "Lower limit of an interval cannot be greater"
            " than the upper limit.")
      << "Lower limit > Upper limit didn't raise an error [10, -5].";

  try {
    Interval i{-10.f, -15.8f};
  } catch (const std::invalid_argument& e) {
    error_message = e.what();
  }

  EXPECT_EQ(error_message, "Lower limit of an interval cannot be greater"
            " than the upper limit.")
      << "Lower limit > Upper limit didn't raise an error [-10, -15.8].";
}

TEST_F(IntervalTest, OperatorEqualsTest) {
  Interval original{-2.7f, 5.9f};
  Interval duplicate = original;
  EXPECT_FLOAT_EQ(duplicate.begin(), -2.7f)
      << "The lower limit should equal -2.7.";
  EXPECT_FLOAT_EQ(duplicate.end(), 5.9f)
      << "The upper limit should equal 5.9.";
  EXPECT_FLOAT_EQ(duplicate.begin(), original.begin())
      << "The beginning of the duplicate should be equal to the original's.";
  EXPECT_FLOAT_EQ(duplicate.end(), original.end())
      << "The end of the duplicate should be equal to the original's.";
}

TEST_F(IntervalTest, LimitsTest) {
  Interval i1{3.5f, 7.8f};
  EXPECT_FLOAT_EQ(i1.begin(), 3.5f)
    << "Interval should start at 3.5.";
  EXPECT_FLOAT_EQ(i1.end(), 7.8f)
    << "Interval should end at 7.8.";

  Interval i2{-2.5f, 4.1f};
  EXPECT_FLOAT_EQ(i2.begin(), -2.5f)
    << "Interval should start at -2.5.";
  EXPECT_FLOAT_EQ(i2.end(), 4.1f)
    << "Interval should end at 4.1.";

  Interval i3{-9.6f, -3.2f};
  EXPECT_FLOAT_EQ(i3.begin(), -9.6f)
    << "Interval should start at -9.6.";
  EXPECT_FLOAT_EQ(i3.end(), -3.2f)
    << "Interval should end at -3.2.";
}

TEST_F(IntervalTest, GetRangeTest) {
  Interval i{2.f, 9.f};
  EXPECT_FLOAT_EQ(i.getRange(), 7.f)
    << "The range between 2 and 9 should be 7.";

  i = Interval(4.5f, 8.f);
  EXPECT_FLOAT_EQ(i.getRange(), 3.5f)
    << "The range between 4.5 and 8 should be 3.5.";
}

TEST_F(IntervalTest, ResetTest) {
  Interval i{5.f, 32.8f};
  i.reset();
  EXPECT_FLOAT_EQ(i.begin(), FLT_MAX)
    << "After resetting, the beginning should equal FLT_MAX.";
  EXPECT_FLOAT_EQ(i.end(), -FLT_MAX)
    << "After resetting, the end should equal -FLT_MAX.";
}

}  // namespace Tests
