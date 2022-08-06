#include "liquid/core/Base.h"
#include "liquid/core/SparseSet.h"

#include "liquid-tests/Testing.h"

class SparseSetTest : public ::testing::Test {
public:
  liquid::SparseSet<uint32_t> sparseSet;

  static constexpr size_t NumItems = 20;
};

using SparseSetDeathTest = SparseSetTest;

using SparseSetIteratorTest = SparseSetTest;

TEST_F(SparseSetTest, PushesItemToTheEndOfTheListIfNoHoles) {
  for (size_t i = 0; i < NumItems; ++i) {
    size_t index = sparseSet.insert(static_cast<uint32_t>(i) * 2);
    EXPECT_EQ(index, (sparseSet.size() - 1));
  }

  for (size_t i = 0; i < NumItems; ++i) {
    EXPECT_EQ(sparseSet.at(i), static_cast<uint32_t>(i) * 2);
  }
}

TEST_F(SparseSetTest, ErasesItemAtIndex) {
  for (uint32_t i = 0; i < NumItems; ++i) {
    size_t index = sparseSet.insert(i * 2);
    EXPECT_EQ(index, sparseSet.size() - 1);
  }

  sparseSet.erase(5);
  sparseSet.erase(9);
  sparseSet.erase(17);

  for (size_t i = 0; i < 5; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }

  EXPECT_FALSE(sparseSet.contains(5));

  for (size_t i = 6; i < 9; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }

  EXPECT_FALSE(sparseSet.contains(9));

  for (size_t i = 10; i < 17; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }

  EXPECT_FALSE(sparseSet.contains(17));

  for (size_t i = 18; i < NumItems; ++i) {
    EXPECT_TRUE(sparseSet.contains(i));
    EXPECT_EQ(sparseSet.at(i), i * 2);
  }
}

TEST_F(SparseSetTest, FillsEmptyHolesOnInsertIfThereAreAny) {
  for (size_t i = 0; i < NumItems; ++i) {
    size_t index = sparseSet.insert(static_cast<uint32_t>(i) * 2);
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
  for (uint32_t i = 0; i < NumItems; ++i) {
    size_t index = sparseSet.insert(i * 2);
    EXPECT_EQ(index, sparseSet.size() - 1);
  }

  sparseSet.erase(5);
  sparseSet.erase(9);
  sparseSet.erase(17);

  EXPECT_EQ(sparseSet.size(), NumItems - 3);
}

TEST_F(SparseSetTest, EmptyReturnsTrueIfSetIsEmpty) {
  for (uint32_t i = 0; i < NumItems; ++i) {
    sparseSet.insert(i * 2);
  }

  for (uint32_t i = 0; i < NumItems; ++i) {
    sparseSet.erase(i);
  }

  EXPECT_TRUE(sparseSet.empty());
}

TEST_F(SparseSetTest, EmptyReturnsFalseIfSetIsNotEmpty) {
  for (uint32_t i = 0; i < NumItems; ++i) {
    size_t index = sparseSet.insert(i * 2);
  }

  EXPECT_FALSE(sparseSet.empty());
}

TEST_F(SparseSetDeathTest, FailsGettingItemIfIndexOutOfBounds) {
  EXPECT_DEATH(sparseSet.at(1000), "out of bounds");
}

TEST_F(SparseSetDeathTest, FailsGettingItemIfNoItemAtIndex) {
  for (uint32_t i = 0; i < NumItems; ++i) {
    sparseSet.insert(i * 2);
  }

  sparseSet.erase(5);

  EXPECT_DEATH(sparseSet.at(5), "data at key");
}

TEST_F(SparseSetTest, FailsGettingItemIfNoItemAtIndex) {
  for (uint32_t i = 0; i < NumItems; ++i) {
    sparseSet.insert(i * 2);
  }

  size_t i = 0;
  for (auto value : sparseSet) {
    EXPECT_EQ(value, i * 2);
    i++;
  }
}
