#include "liquid/core/Base.h"
#include "liquid/rhi/PhysicalDeviceInformation.h"

#include "liquid-tests/Testing.h"

TEST(PhysicalDeviceInformationTest, SetsNameTypePropertiesAndLimits) {
  liquid::rhi::PhysicalDeviceLimits limits{};
  limits.minUniformBufferOffsetAlignment = 64;

  liquid::rhi::PhysicalDeviceProperties properties{
      liquid::rhi::PhysicalDeviceType::CPU, "Mock"};

  liquid::rhi::PhysicalDeviceInformation info("Test Device", properties,
                                              limits);

  EXPECT_EQ(info.getName(), "Test Device");
  EXPECT_EQ(info.getType(), liquid::rhi::PhysicalDeviceType::CPU);
  EXPECT_EQ(info.getProperties().apiName, "Mock");

  EXPECT_EQ(info.getLimits().minUniformBufferOffsetAlignment, 64);
}
