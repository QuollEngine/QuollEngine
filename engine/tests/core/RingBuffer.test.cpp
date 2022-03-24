#include "liquid/core/Base.h"
#include "liquid/core/RingBuffer.h"

#include <gtest/gtest.h>

struct Data {
  int val = 0;
};

class RingBufferTest : public ::testing::Test {
public:
  liquid::RingBuffer<Data> buffer{5};
};

using RingBufferDeathTest = RingBufferTest;

TEST_F(RingBufferTest, PushesItemToTheQueue) {
  buffer.push({1});
  EXPECT_EQ(buffer.size(), 1);
}

TEST_F(RingBufferDeathTest, PushingFailsIfQueueIsFull) {
  // add five items
  for (int i = 0; i < 5; ++i) {
    buffer.push({i});
  }
  EXPECT_DEATH(buffer.push({}), ".*");
}

TEST_F(RingBufferTest, FrontReturnsFirstItemInQueue) {
  buffer.push({5});
  EXPECT_EQ(buffer.front().val, 5);
}

TEST_F(RingBufferDeathTest, FrontThrowsErrorIfQueueIsEmpty) {
  EXPECT_DEATH(buffer.front(), ".*");
}

TEST_F(RingBufferTest, PopsItemFromTheFrontOfTheQueue) {
  // add two items
  buffer.push({2});
  buffer.push({3});
  EXPECT_EQ(buffer.front().val, 2);
  EXPECT_EQ(buffer.size(), 2);

  // remove one
  buffer.pop();
  EXPECT_EQ(buffer.front().val, 3);
  EXPECT_EQ(buffer.size(), 1);
}

TEST_F(RingBufferDeathTest, PoppingFailsIfQueueIsEmpty) {
  EXPECT_DEATH(buffer.pop(), ".*");
}

TEST_F(RingBufferTest, ManyOperations) {
  liquid::RingBuffer<Data> bigBuffer(150);

  static constexpr int NUM_ADDITIONS = 100;
  for (int i = 0; i < NUM_ADDITIONS; ++i) {
    bigBuffer.push({i});
  }

  EXPECT_FALSE(bigBuffer.empty());
  EXPECT_EQ(bigBuffer.size(), NUM_ADDITIONS);
  int i = 0;
  while (!bigBuffer.empty()) {
    EXPECT_EQ(bigBuffer.front().val, i++);
    bigBuffer.pop();
  }

  EXPECT_EQ(i, NUM_ADDITIONS);
  EXPECT_TRUE(bigBuffer.empty());
}
