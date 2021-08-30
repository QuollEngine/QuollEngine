#include <gtest/gtest.h>
#include "profiler/PhysicalDeviceInformation.h"

TEST(PhysicalDeviceInformationTest, SetsNameTypePropertiesAndLimits) {
  liquid::PhysicalDeviceInformation info(
      "Test Device", liquid::PhysicalDeviceType::DISCRETE_GPU,
      {{"driverVersion", liquid::Property(120u)},
       {"apiVersion", liquid::Property(1u)}},
      {{"maxViewports", liquid::Property(1u)}});

  EXPECT_EQ(info.getName(), "Test Device");
  EXPECT_EQ(info.getType(), liquid::PhysicalDeviceType::DISCRETE_GPU);

  EXPECT_EQ(info.getProperties().size(), 2);
  EXPECT_EQ(info.getProperties()[0].first, "driverVersion");
  EXPECT_EQ(info.getProperties()[0].second.getValue<uint32_t>(), 120);

  EXPECT_EQ(info.getProperties()[1].first, "apiVersion");
  EXPECT_EQ(info.getProperties()[1].second.getValue<uint32_t>(), 1);

  EXPECT_EQ(info.getLimits().size(), 1);
  EXPECT_EQ(info.getLimits()[0].first, "maxViewports");
  EXPECT_EQ(info.getLimits()[0].second.getValue<uint32_t>(), 1);
}
