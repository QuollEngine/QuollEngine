#include "quoll/core/Base.h"
#include "quoll/rhi/PhysicalDeviceInformation.h"
#include "quoll-tests/Testing.h"

TEST(PhysicalDeviceInformationTest, SetsNameTypePropertiesAndLimits) {
  quoll::rhi::PhysicalDeviceLimits limits{};
  limits.minUniformBufferOffsetAlignment = 64;

  quoll::rhi::PhysicalDeviceProperties properties{
      quoll::rhi::PhysicalDeviceType::CPU, "Mock"};

  quoll::rhi::PhysicalDeviceInformation info("Test Device", properties, limits);

  EXPECT_EQ(info.getName(), "Test Device");
  EXPECT_EQ(info.getType(), quoll::rhi::PhysicalDeviceType::CPU);
  EXPECT_EQ(info.getProperties().apiName, "Mock");

  EXPECT_EQ(info.getLimits().minUniformBufferOffsetAlignment, 64);
}
