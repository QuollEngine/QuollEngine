#include <gtest/gtest.h>

#include "core/Base.h"
#include "../mocks/TestBuffer.h"
#include "profiler/StatsManager.h"

TEST(HardwareBufferTest, CreatesBuffer) {
  liquid::StatsManager statsManager;
  TestBuffer buffer(liquid::HardwareBuffer::VERTEX, 21121, statsManager);
  EXPECT_EQ(buffer.getType(), liquid::HardwareBuffer::VERTEX);
  EXPECT_EQ(buffer.getBufferSize(), 21121);
}

TEST(HardwareBufferTest, CollectsStatisticsAfterBufferDeletion) {
  liquid::StatsManager statsManager;
  TestBuffer buffer(liquid::HardwareBuffer::VERTEX, 7 * sizeof(float),
                    statsManager);

  {
    TestBuffer buffer(liquid::HardwareBuffer::VERTEX, 10 * sizeof(float),
                      statsManager);

    EXPECT_EQ(statsManager.getAllocatedBuffersCount(), 2);
    EXPECT_EQ(statsManager.getAllocatedBuffersSize(), 17 * sizeof(float));
  }

  EXPECT_EQ(statsManager.getAllocatedBuffersCount(), 1);
  EXPECT_EQ(statsManager.getAllocatedBuffersSize(), 7 * sizeof(float));
}
