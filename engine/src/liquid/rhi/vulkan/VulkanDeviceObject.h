#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanPhysicalDevice.h"

namespace liquid::experimental {

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

private:
  VkDevice mDevice = VK_NULL_HANDLE;
};

} // namespace liquid::experimental
