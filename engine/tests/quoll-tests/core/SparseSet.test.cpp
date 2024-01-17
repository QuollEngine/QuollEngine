#include "quoll/core/Base.h"
#include "quoll/core/SparseSet.h"
#include "quoll-tests/Testing.h"

class SparseSetTest : public ::testing::Test {
public:
  quoll::SparseSet<u32> sparseSet;

  static constexpr usize NumItems = 20;
};

using SparseSetDeathTest = SparseSetTest;

using SparseSetIteratorTest = SparseSetTest;

TEST_F(SparseSetTest, PushesItemToTheEndOfTheListIfNoHoles) {
  for (usize i = 0; i < NumItems; ++i) {
    usize index = sparseSet.insert(static_cast<u32>(i) * 2);
    EXPECT_EQ(index, (sparseSet.size() - 1));
  }

  for (usize i = 0; i < NumItems; ++i) {
    EXPECT_EQ(sparseSet.at(i), static_cast<u32>(i) * 2);
  }
}

TEST_F(SparseSetTest, ErasesItemAtIndex) {
  for (u32 i = 0; i < NumItems; ++i) {
    usize index = sparseSet.insert(i * 2);
    EXPECT_EQ(index, sparseSet.size() - 1);
  }

  sparseSet.erase(5);
  sparseSet.erase(9);
  sparseSet.erase(17);

  for (usize i = 0; i < 5; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }

  EXPECT_FALSE(sparseSet.contains(5));

  for (usize i = 6; i < 9; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }

  EXPECT_FALSE(sparseSet.contains(9));

  for (usize i = 10; i < 17; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }

  EXPECT_FALSE(sparseSet.contains(17));

  for (usize i = 18; i < NumItems; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }
}

TEST_F(SparseSetTest, OutOfOrderAddAndEraseDoesNotCauseInvalidState) {
  for (u32 i = 0; i < NumItems; ++i) {
    usize index0 = sparseSet.insert(i);
    usize index1 = sparseSet.insert(NumItems + i);

    sparseSet.erase(index0);
    sparseSet.erase(index1);
  }

  for (u32 i = 0; i < NumItems; ++i) {
    EXPECT_FALSE(sparseSet.contains(i));
    EXPECT_FALSE(sparseSet.contains(NumItems + i));
  }
}

TEST_F(SparseSetTest, FillsEmptyHolesOnInsertIfThereAreAny) {
  for (usize i = 0; i < NumItems; ++i) {
    usize index = sparseSet.insert(static_cast<u32>(i) * 2);
    EXPECT_EQ(index, sparseSet.size() - 1);
  }

  sparseSet.erase(5);
  sparseSet.erase(9);
  sparseSet.erase(17);

  EXPECT_EQ(sparseSet.insert(55), 17);
  EXPECT_EQ(sparseSet.at(17), 55);

  EXPECT_EQ(sparseSet.insert(65), 9);
  EXPECT_EQ(sparseSet.at(9), 65);

  EXPECT_EQ(sparseSet.insert(85), 5);
  EXPECT_EQ(sparseSet.at(5), 85);

  auto newIndex = sparseSet.insert(110);
  EXPECT_EQ(newIndex, sparseSet.size() - 1);
  EXPECT_EQ(sparseSet.at(sparseSet.size() - 1), 110);
}

TEST_F(SparseSetTest, GetsSize) {
  for (u32 i = 0; i < NumItems; ++i) {
    usize index = sparseSet.insert(i * 2);
    EXPECT_EQ(index, sparseSet.size() - 1);
  }

  sparseSet.erase(5);
  sparseSet.erase(9);
  sparseSet.erase(17);

  EXPECT_EQ(sparseSet.size(), NumItems - 3);
}

TEST_F(SparseSetTest, EmptyReturnsTrueIfSetIsEmpty) {
  for (u32 i = 0; i < NumItems; ++i) {
    sparseSet.insert(i * 2);
  }

  for (u32 i = 0; i < NumItems; ++i) {
    sparseSet.erase(i);
  }

  EXPECT_TRUE(sparseSet.empty());
}

TEST_F(SparseSetTest, EmptyReturnsFalseIfSetIsNotEmpty) {
  for (u32 i = 0; i < NumItems; ++i) {
    usize index = sparseSet.insert(i * 2);
  }

  EXPECT_FALSE(sparseSet.empty());
}

TEST_F(SparseSetDeathTest, FailsGettingItemIfIndexOutOfBounds) {
  EXPECT_DEATH(sparseSet.at(1000), "out of bounds");
}

TEST_F(SparseSetDeathTest, FailsGettingItemIfNoItemAtIndex) {
  for (u32 i = 0; i < NumItems; ++i) {
    sparseSet.insert(i * 2);
  }

  sparseSet.erase(5);

  EXPECT_DEATH(sparseSet.at(5), "data at key");
}

TEST_F(SparseSetTest, FailsGettingItemIfNoItemAtIndex) {
  for (u32 i = 0; i < NumItems; ++i) {
    sparseSet.insert(i * 2);
  }

  usize i = 0;
  for (auto value : sparseSet) {
    EXPECT_EQ(value, i * 2);
    i++;
  }
}
