#pragma once

#include "VulkanHeaders.h"

namespace quoll::rhi {

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
  inline const std::array<u32, 3> toArray() const {
    return {getGraphicsFamily(), getTransferFamily(), getPresentFamily()};
  }

  /**
   * @brief Get graphics queue index
   *
   * @return Graphics queue index
   */
  inline u32 getGraphicsFamily() const { return mGraphicsFamily.value(); }

  /**
   * @brief Get present queue index
   *
   * @return Present queue index
   */
  inline u32 getPresentFamily() const { return mPresentFamily.value(); }

  /**
   * @brief Get transfer queue index
   *
   * @return Transfer queue index
   */
  inline u32 getTransferFamily() const { return mTransferFamily.value(); }

private:
  std::optional<u32> mGraphicsFamily;
  std::optional<u32> mPresentFamily;
  std::optional<u32> mTransferFamily;
};

} // namespace quoll::rhi
