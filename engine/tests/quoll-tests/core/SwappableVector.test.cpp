#include "quoll/core/Base.h"
#include "quoll/core/SwappableVector.h"
#include "quoll-tests/Testing.h"

struct Data {
  int val = 0;
};

class SwappableVectorTest : public ::testing::Test {
public:
  quoll::SwappableVector<Data> buffer;
};

using SwappableVectorDeathTest = SwappableVectorTest;

TEST_F(SwappableVectorTest, PushesItemToTheEndOfArrayIfNothingIsRemoved) {
  buffer.push_back({1});
  EXPECT_EQ(buffer.at(0).val, 1);
  EXPECT_EQ(buffer.size(), 1);
}

TEST_F(SwappableVectorTest, ErasingItemSwapsItWithLastItemAndDecreasesSize) {
  buffer.push_back({1});
  buffer.push_back({2});
  buffer.push_back({3});
  EXPECT_EQ(buffer.at(0).val, 1);
  EXPECT_EQ(buffer.size(), 3);

  buffer.erase(0);
  EXPECT_EQ(buffer.at(0).val, 3);
  EXPECT_EQ(buffer.size(), 2);
}

TEST_F(SwappableVectorDeathTest, ErasingNonExistentItemFails) {
  EXPECT_DEATH(buffer.erase(0), ".*");
}

TEST_F(SwappableVectorDeathTest, AccessingNonExistentItemFails) {
  EXPECT_DEATH(buffer.at(0), ".*");
}

TEST_F(SwappableVectorTest, BeginReturnsFirstItemInVector) {
  buffer.push_back({7});
  buffer.push_back({2});

  EXPECT_EQ((*buffer.begin()).val, 7);
}

TEST_F(SwappableVectorDeathTest, EndReturnsLastItemInTheList) {
  buffer.push_back({7});
  buffer.push_back({2});

  EXPECT_DEATH((*buffer.end()), ".*");
}

TEST_F(SwappableVectorTest, RangeForLoopWorks) {
  buffer.push_back({5});
  buffer.push_back({7});
  buffer.push_back({8});

  usize i = 0;
  for (auto &x : buffer) {
    EXPECT_EQ(x.val, buffer.at(i).val);
    i++;
  }

  EXPECT_EQ(i, buffer.size());
}
