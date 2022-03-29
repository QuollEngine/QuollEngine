#include "liquid/core/Base.h"
#include "liquid/rhi/DeviceStats.h"

#include <gtest/gtest.h>

class DeviceStatsTest : public ::testing::Test {
public:
  liquid::rhi::DeviceStats stats;
};

TEST_F(DeviceStatsTest, AddsDrawCalls) {
  stats.addDrawCall(80);
  stats.addDrawCall(125);

  EXPECT_EQ(stats.getDrawCallsCount(), 2);
  EXPECT_EQ(stats.getDrawnPrimitivesCount(), 205);
}

TEST_F(DeviceStatsTest, ResetsDrawCalls) {
  stats.addDrawCall(80);
  stats.addDrawCall(125);

  stats.resetDrawCalls();
  EXPECT_EQ(stats.getDrawCallsCount(), 0);
  EXPECT_EQ(stats.getDrawnPrimitivesCount(), 0);
}
