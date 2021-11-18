#include "core/Base.h"
#include "profiler/StatsManager.h"

#include <gtest/gtest.h>

TEST(StatsManagerTest, AddsDrawCalls) {
  liquid::StatsManager manager;

  manager.addDrawCall(80);
  manager.addDrawCall(125);

  EXPECT_EQ(manager.getDrawCallsCount(), 2);
  EXPECT_EQ(manager.getDrawnPrimitivesCount(), 205);
}

TEST(StatsManagerTest, ResetsDrawCalls) {
  liquid::StatsManager manager;

  manager.addDrawCall(80);
  manager.addDrawCall(125);

  manager.resetDrawCalls();
  EXPECT_EQ(manager.getDrawCallsCount(), 0);
  EXPECT_EQ(manager.getDrawnPrimitivesCount(), 0);
}

TEST(StatsManagerTest, RemovesTextureStats) {
  liquid::StatsManager manager;

  manager.addTexture(250);
  manager.addTexture(740);

  manager.removeTexture(250);

  EXPECT_EQ(manager.getAllocatedTexturesCount(), 1);
  EXPECT_EQ(manager.getAllocatedTexturesSize(), 740);
}

TEST(StatsManagerTest, AddsBufferStats) {
  liquid::StatsManager manager;

  manager.addBuffer(250);
  manager.addBuffer(740);

  EXPECT_EQ(manager.getAllocatedBuffersCount(), 2);
  EXPECT_EQ(manager.getAllocatedBuffersSize(), 990);
}

TEST(StatsManagerTest, RemovesBufferStats) {
  liquid::StatsManager manager;

  manager.addBuffer(250);
  manager.addBuffer(740);

  manager.removeBuffer(250);

  EXPECT_EQ(manager.getAllocatedBuffersCount(), 1);
  EXPECT_EQ(manager.getAllocatedBuffersSize(), 740);
}
