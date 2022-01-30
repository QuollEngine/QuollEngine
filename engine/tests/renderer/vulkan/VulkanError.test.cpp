#include "liquid/core/Base.h"
#include <gtest/gtest.h>
#include <vulkan/vulkan.hpp>
#include "liquid/renderer/vulkan/VulkanError.h"

TEST(VulkanErrorTest, ShowsErrorMessageWithoutCodeIfSuccess) {
  liquid::VulkanError error("Test error message", VK_SUCCESS);

  EXPECT_EQ(liquid::String(error.what()), "[VulkanError] Test error message");
}

TEST(VulkanErrorTest, ShowsErrorMessageWithCodeAndExplanationIfCodeExists) {
  liquid::VulkanError error("Test error message", VK_ERROR_DEVICE_LOST);

  EXPECT_EQ(liquid::String(error.what()),
            "[VulkanError] Test error message: Device lost (code: -4)");
}

TEST(VulkanErrorTest,
     ShowsErrorMessageWithCodeWithUnknownErrorIfCodeDoesNotExist) {
  liquid::VulkanError error("Test error message", (VkResult)12345);

  EXPECT_EQ(liquid::String(error.what()),
            "[VulkanError] Test error message: Unknown Error (code: 12345)");
}

TEST(CheckForVulkanError, DoesNothingIfSuccess) {
  EXPECT_NO_THROW(
      liquid::checkForVulkanError(VK_SUCCESS, "Test error message"));
}

TEST(CheckForVulkanError, ThrowsVulkanErrorIfNotSuccess) {
  EXPECT_THROW(
      liquid::checkForVulkanError(VK_ERROR_DEVICE_LOST, "Test error message"),
      liquid::VulkanError);
}
