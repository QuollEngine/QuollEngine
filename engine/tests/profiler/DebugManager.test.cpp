#include <gtest/gtest.h>
#include "profiler/DebugManager.h"

TEST(DebugManager, SetWireframeMode) {
  liquid::DebugManager debugManager;

  EXPECT_EQ(debugManager.getWireframeMode(), false);

  debugManager.setWireframeMode(true);
  EXPECT_EQ(debugManager.getWireframeMode(), true);

  debugManager.setWireframeMode(false);
  EXPECT_EQ(debugManager.getWireframeMode(), false);
}
