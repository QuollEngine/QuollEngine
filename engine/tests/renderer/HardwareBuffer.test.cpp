#include <gtest/gtest.h>

#include "core/Base.h"
#include "../mocks/TestBuffer.h"
#include "profiler/StatsManager.h"

TEST(HardwareBufferTest, SetsVertexBufferOnVertexArrayConstruct) {
  std::vector<liquid::Vertex> vertices{{1.0, 1.0, 1.0}, {1.0, 2.0, 3.0}};
  TestBuffer buffer(vertices);

  EXPECT_EQ(buffer.getType(), liquid::HardwareBuffer::VERTEX);
  EXPECT_EQ(buffer.getBufferSize(), 2 * sizeof(liquid::Vertex));
  EXPECT_EQ(buffer.getItemSize(), 2);
}

TEST(HardwareBufferTest, CollectsStatisticsForVertexBuffer) {
  liquid::SharedPtr<liquid::StatsManager> manager(new liquid::StatsManager);
  std::vector<liquid::Vertex> vertices{{1.0, 1.0, 1.0}, {1.0, 2.0, 3.0}};
  TestBuffer buffer(vertices, manager);

  EXPECT_EQ(manager->getAllocatedBuffersCount(), 1);
  EXPECT_EQ(manager->getAllocatedBuffersSize(), sizeof(liquid::Vertex) * 2);
}

TEST(HardwareBufferTest, SetsIndexBufferOnUintArrayConstruct) {
  std::vector<uint32_t> indices{0, 1, 2, 3, 4, 5};
  TestBuffer buffer(indices);

  EXPECT_EQ(buffer.getType(), liquid::HardwareBuffer::INDEX);
  EXPECT_EQ(buffer.getBufferSize(), 6 * sizeof(uint32_t));
  EXPECT_EQ(buffer.getItemSize(), 6);
}

TEST(HardwareBufferTest, CollectsStatisticsForIndexBuffer) {
  liquid::SharedPtr<liquid::StatsManager> manager(new liquid::StatsManager);
  std::vector<uint32_t> indices{0, 1, 2, 3, 4, 5};
  TestBuffer buffer(indices, manager);

  EXPECT_EQ(manager->getAllocatedBuffersCount(), 1);
  EXPECT_EQ(manager->getAllocatedBuffersSize(), sizeof(uint32_t) * 6);
}

TEST(HardwareBufferTest, SetsUniformBufferOnSizeConstruct) {
  TestBuffer buffer(10 * sizeof(float));

  EXPECT_EQ(buffer.getType(), liquid::HardwareBuffer::UNIFORM);
  EXPECT_EQ(buffer.getBufferSize(), 10 * sizeof(float));
  EXPECT_EQ(buffer.getItemSize(), 0);
}

TEST(HardwareBufferTest, CollectsStatisticsForUniformBuffer) {
  liquid::SharedPtr<liquid::StatsManager> manager(new liquid::StatsManager);
  TestBuffer buffer(10 * sizeof(float), manager);

  EXPECT_EQ(manager->getAllocatedBuffersCount(), 1);
  EXPECT_EQ(manager->getAllocatedBuffersSize(), 10 * sizeof(float));
}

TEST(HardwareBufferTest, CollectsStatisticsAfterBufferDeletion) {
  liquid::SharedPtr<liquid::StatsManager> manager(new liquid::StatsManager);
  TestBuffer buffer(7 * sizeof(float), manager);

  {
    TestBuffer buffer(10 * sizeof(float), manager);

    EXPECT_EQ(manager->getAllocatedBuffersCount(), 2);
    EXPECT_EQ(manager->getAllocatedBuffersSize(), 17 * sizeof(float));
  }

  EXPECT_EQ(manager->getAllocatedBuffersCount(), 1);
  EXPECT_EQ(manager->getAllocatedBuffersSize(), 7 * sizeof(float));
}
