#include "liquid/core/Base.h"

#include "liquid-tests/Testing.h"

#include <vulkan/vulkan.hpp>
#include "liquid/rhi/vulkan/VulkanError.h"

TEST(CheckForVulkanError, DoesNothingIfSuccess) {
  liquid::rhi::checkForVulkanError(VK_SUCCESS, "Test error message");
}

TEST(CheckForVulkanErrorDeathTest, ThrowsVulkanErrorIfNotSuccess) {
  EXPECT_DEATH(liquid::rhi::checkForVulkanError(VK_ERROR_DEVICE_LOST,
                                                "Test error message"),
               ".*");
}
