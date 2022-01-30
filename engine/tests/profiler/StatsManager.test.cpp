#include "liquid/core/Base.h"
#include "liquid/profiler/StatsManager.h"

#include <gtest/gtest.h>

class StatsManagerTest : public ::testing::Test {
public:
  liquid::StatsManager manager;
};

TEST_F(StatsManagerTest, AddsDrawCalls) {
  manager.addDrawCall(80);
  manager.addDrawCall(125);

  EXPECT_EQ(manager.getDrawCallsCount(), 2);
  EXPECT_EQ(manager.getDrawnPrimitivesCount(), 205);
}

TEST_F(StatsManagerTest, ResetsDrawCalls) {
  manager.addDrawCall(80);
  manager.addDrawCall(125);

  manager.resetDrawCalls();
  EXPECT_EQ(manager.getDrawCallsCount(), 0);
  EXPECT_EQ(manager.getDrawnPrimitivesCount(), 0);
}

TEST_F(StatsManagerTest, RemovesTextureStats) {
  manager.addTexture(250);
  manager.addTexture(740);

  manager.removeTexture(250);

  EXPECT_EQ(manager.getAllocatedTexturesCount(), 1);
  EXPECT_EQ(manager.getAllocatedTexturesSize(), 740);
}

TEST_F(StatsManagerTest, AddsBufferStats) {
  manager.addBuffer(250);
  manager.addBuffer(740);

  EXPECT_EQ(manager.getAllocatedBuffersCount(), 2);
  EXPECT_EQ(manager.getAllocatedBuffersSize(), 990);
}

TEST_F(StatsManagerTest, RemovesBufferStats) {
  manager.addBuffer(250);
  manager.addBuffer(740);

  manager.removeBuffer(250);

  EXPECT_EQ(manager.getAllocatedBuffersCount(), 1);
  EXPECT_EQ(manager.getAllocatedBuffersSize(), 740);
}

TEST_F(StatsManagerTest, CollectsFPS) {
  manager.collectFPS(60);
  EXPECT_EQ(manager.getFPS(), 60);
}
