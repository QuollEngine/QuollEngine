#pragma once

namespace liquid {

/**
 * @brief Vulkan Error
 *
 * This exception should be used for all
 * Vulkan related fatal errors
 */
class VulkanError : public std::runtime_error {
public:
  /**
   * @brief Error constructor
   *
   * @param what Error message
   * @param resultCode Vulkan result code
   */
  VulkanError(const String &what, VkResult resultCode);
};

/**
 * @brief Throws Vulkan error if result is not success
 *
 * @param resultCode Vulkan result code
 * @param errorMessage Error message to assert when result code is not success
 */
inline void checkForVulkanError(VkResult resultCode,
                                const String &errorMessage) {
  LIQUID_ASSERT(resultCode == VK_SUCCESS,
                VulkanError(errorMessage, resultCode).what());
}

} // namespace liquid
