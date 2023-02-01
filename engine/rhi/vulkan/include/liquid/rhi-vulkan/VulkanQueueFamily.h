#pragma once

#include "VulkanHeaders.h"

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
   * Queue family is complete if it has graphics,
   * transfer, and present family available
   *
   * @retval true Complete
   * @retval false Not complete
   */
  inline bool isComplete() const {
    return mGraphicsFamily.has_value() && mPresentFamily.has_value() &&
           mTransferFamily.has_value();
  }

  /**
   * @brief Returns queue families in array
   *
   * @return Array of all queue families
   */
  inline const std::array<uint32_t, 3> toArray() const {
    return {getGraphicsFamily(), getTransferFamily(), getPresentFamily()};
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

  /**
   * @brief Get transfer queue index
   *
   * @return Transfer queue index
   */
  inline uint32_t getTransferFamily() const { return mTransferFamily.value(); }

private:
  std::optional<uint32_t> mGraphicsFamily;
  std::optional<uint32_t> mPresentFamily;
  std::optional<uint32_t> mTransferFamily;
};

} // namespace liquid::rhi
