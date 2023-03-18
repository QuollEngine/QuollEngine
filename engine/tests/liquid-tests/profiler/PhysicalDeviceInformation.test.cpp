#include "liquid/core/Base.h"
#include "liquid/rhi/PhysicalDeviceInformation.h"

#include "liquid-tests/Testing.h"

TEST(PhysicalDeviceInformationTest, SetsNameTypePropertiesAndLimits) {
  liquid::rhi::PhysicalDeviceInformation::Limits limits{};
  limits.minUniformBufferOffsetAlignment = 64;

  liquid::rhi::PhysicalDeviceInformation info(
      "Test Device", liquid::rhi::PhysicalDeviceType::DiscreteGPU,
      {{"driverVersion", liquid::Property(120u)},
       {"apiVersion", liquid::Property(1u)}},
      {{"maxViewports", liquid::Property(1u)}}, limits);

  EXPECT_EQ(info.getName(), "Test Device");
  EXPECT_EQ(info.getType(), liquid::rhi::PhysicalDeviceType::DiscreteGPU);

  EXPECT_EQ(info.getProperties().size(), 2);
  EXPECT_EQ(info.getProperties()[0].first, "driverVersion");
  EXPECT_EQ(info.getProperties()[0].second.getValue<uint32_t>(), 120);

  EXPECT_EQ(info.getProperties()[1].first, "apiVersion");
  EXPECT_EQ(info.getProperties()[1].second.getValue<uint32_t>(), 1);

  EXPECT_EQ(info.getRawLimits().size(), 1);
  EXPECT_EQ(info.getRawLimits()[0].first, "maxViewports");
  EXPECT_EQ(info.getRawLimits()[0].second.getValue<uint32_t>(), 1);

  EXPECT_EQ(info.getLimits().minUniformBufferOffsetAlignment, 64);
}
