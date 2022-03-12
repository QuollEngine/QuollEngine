#include "liquid/core/Base.h"
#include <gtest/gtest.h>
#include <vulkan/vulkan.hpp>
#include "liquid/renderer/vulkan/VulkanError.h"

TEST(CheckForVulkanError, DoesNothingIfSuccess) {
  liquid::checkForVulkanError(VK_SUCCESS, "Test error message");
}

TEST(CheckForVulkanErrorDeathTest, ThrowsVulkanErrorIfNotSuccess) {
  EXPECT_DEATH(
      liquid::checkForVulkanError(VK_ERROR_DEVICE_LOST, "Test error message"),
      ".*");
}
