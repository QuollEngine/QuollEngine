#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

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
    return graphicsFamily.has_value() && presentFamily.has_value();
  }

  /**
   * @brief Returns queue families in array
   *
   * @return Array with graphics and present family indices
   */
  inline const std::vector<uint32_t> toArray() const {
    return {graphicsFamily.value(), presentFamily.value()};
  }

public:
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
};

} // namespace liquid
