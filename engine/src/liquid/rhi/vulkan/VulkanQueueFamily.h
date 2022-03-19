#pragma once

#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

/**
 * @brief Vulkan queue family indices
 *
 * Used to identify indices for
 * different queue families
 */
class VulkanQueueFamily {
public:
  /**
   * @brief Default constructor
   */
  VulkanQueueFamily() = default;

  /**
   * @brief Constructor for Vulkan queue family
   *
   * @param device Vulkan physical Device handle
   * @param surface Vulkan surface handle
   */
  VulkanQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface);

  /**
   * @brief Checks if queue family is complete
   *
   * Queue family is complete if it has both graphics
   * and present family available
   *
   * @retval true Complete
   * @retval false Not complete
   */
  inline bool isComplete() const {
    return mGraphicsFamily.has_value() && mPresentFamily.has_value();
  }

  /**
   * @brief Returns queue families in array
   *
   * @return Array with graphics and present family indices
   */
  inline const std::vector<uint32_t> toArray() const {
    return {mGraphicsFamily.value(), mPresentFamily.value()};
  }

  /**
   * @brief Get graphics queue index
   *
   * @return Graphics queue index
   */
  inline uint32_t getGraphicsFamily() const { return mGraphicsFamily.value(); }

  /**
   * @brief Get present queue index
   *
   * @return Present queue index
   */
  inline uint32_t getPresentFamily() const { return mPresentFamily.value(); }

private:
  std::optional<uint32_t> mGraphicsFamily;
  std::optional<uint32_t> mPresentFamily;
};

} // namespace liquid::rhi
