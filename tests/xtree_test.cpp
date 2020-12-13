#include "XTree.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Tests {

class XTreeTest : public ::testing::Test {
 protected:
  XTreeTest() {}
  virtual ~XTreeTest() {}

  void SetUp() override {}
  void TearDown() override {}
};


TEST_F(XTreeTest, ConstructorTest) {
  XTree<2, int, 6> xtree;
  EXPECT_EQ(xtree.size(), size_t(0))
      << "The size of a new xtree should equal zero.";
}

TEST_F(XTreeTest, XNodeInsertTest) {
  XTree<2, int, 6> xtree;
  Hyperrectangle<2> h;

  for (int i = 0; i < 6; ++i) {
    h[0] = Interval(i, i);
    h[1] = Interval(i + 1, i + 1);
    xtree.insert(h, i);
  }
  EXPECT_EQ(xtree.size(), size_t(6))
      << "The size of the xtree after inserting one element should equal 6.";

  h[0] = Interval(7, 7);
  h[1] = Interval(8, 8);
  xtree.insert(h, 7);
  EXPECT_EQ(xtree.size(), size_t(7))
      << "The size of the xtree after inserting one element should equal 7.";
  EXPECT_EQ(xtree.root->size, size_t(2))
      << "The size of the xtree after the split should equal 7.";
  EXPECT_EQ((*xtree.root)[0].child_pointer->size, size_t(5))
      << "The size of the xtree's first child after the split should equal 3.";
  EXPECT_EQ((*xtree.root)[1].child_pointer->size, size_t(2))
      << "The size of the xtree's second child after the split should equal 4.";
}

}  // namespace Tests
