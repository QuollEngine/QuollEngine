#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanPhysicalDevice.h"

namespace liquid::rhi {

/**
 * @brief Vulkan device
 */
class VulkanDeviceObject {
public:
  /**
   * @brief Create device object
   *
   * @param physicalDevice Physical device
   */
  VulkanDeviceObject(const VulkanPhysicalDevice &physicalDevice);

  /**
   * @brief Destroy device object
   */
  ~VulkanDeviceObject();

  VulkanDeviceObject(const VulkanDeviceObject &) = delete;
  VulkanDeviceObject &operator=(const VulkanDeviceObject &) = delete;
  VulkanDeviceObject(VulkanDeviceObject &&) = delete;
  VulkanDeviceObject &operator=(VulkanDeviceObject &&) = delete;

  /**
   * @brief Get Vulkan device handle
   *
   * @return Vulkan device handle
   */
  inline operator VkDevice() const { return mDevice; }

  /**
   * @brief Get Vulkan device handle
   *
   * @return Vulkan device handle
   */
  inline VkDevice getVulkanHandle() const { return mDevice; }

private:
  VkDevice mDevice = VK_NULL_HANDLE;
};

} // namespace liquid::rhi
