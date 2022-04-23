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
  EXPECT_EQ(stats.getCommandCallsCount(), 2);
}

TEST_F(DeviceStatsTest, AddCommandCalls) {
  stats.addCommandCall();
  stats.addCommandCall();

  EXPECT_EQ(stats.getDrawCallsCount(), 0);
  EXPECT_EQ(stats.getDrawnPrimitivesCount(), 0);
  EXPECT_EQ(stats.getCommandCallsCount(), 2);
}

TEST_F(DeviceStatsTest, ResetsCalls) {
  stats.addDrawCall(80);
  stats.addDrawCall(125);
  stats.addCommandCall();

  stats.resetCalls();
  EXPECT_EQ(stats.getDrawCallsCount(), 0);
  EXPECT_EQ(stats.getDrawnPrimitivesCount(), 0);
  EXPECT_EQ(stats.getCommandCallsCount(), 0);
}
