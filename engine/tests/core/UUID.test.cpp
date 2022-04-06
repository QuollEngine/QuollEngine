#include "liquid/core/Base.h"
#include "liquid/core/UUID.h"

#include <gtest/gtest.h>

class UUIDTest : public ::testing::Test {};

TEST_F(UUIDTest, CreatesValidUUIDFromString) {
  liquid::UUID uuid("656f436f-f32d-482e-9fab-c9b00b879697");

  EXPECT_EQ(uuid.toString(), "656f436f-f32d-482e-9fab-c9b00b879697");
  EXPECT_TRUE(uuid.isValid());
}

TEST_F(UUIDTest, CreatesInvalidUUIDFromString) {
  liquid::UUID uuid("656f436f");

  EXPECT_EQ(uuid.toString(), "656f436f");
  EXPECT_FALSE(uuid.isValid());
}

TEST_F(UUIDTest, ComparesTwoUUIDs) {
  liquid::UUID uuid1("656f436f-f32d-482e-9fab-c9b00b879697");
  liquid::UUID uuid2("656f436f-f32d-482e-9fab-c9b00b879697");
  liquid::UUID uuid3("656f436f-f32d-482e-9fab-c9b00b879695");

  EXPECT_EQ(uuid1, uuid2);
  EXPECT_NE(uuid1, uuid3);
}
