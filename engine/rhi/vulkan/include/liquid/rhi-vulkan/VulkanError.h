#pragma once

namespace liquid::rhi {

/**
 * @brief Create Vulkan error message
 *
 * @param resultCode Vulkan result code
 * @param errorMessage Custom error message
 * @return Error message
 */
String createVulkanErrorMessage(VkResult resultCode,
                                const String &errorMessage);

/**
 * @brief Throws Vulkan error if result is not success
 *
 * @param resultCode Vulkan result code
 * @param errorMessage Error message to assert when result code is not success
 */
inline void checkForVulkanError(VkResult resultCode,
                                const String &errorMessage) {
  LIQUID_ASSERT(resultCode == VK_SUCCESS,
                createVulkanErrorMessage(resultCode, errorMessage));
  if (resultCode != VK_SUCCESS) {
    std::cout << createVulkanErrorMessage(resultCode, errorMessage);
    std::terminate();
  }
}

} // namespace liquid::rhi
