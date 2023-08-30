#pragma once

#include "VulkanHeaders.h"
#include "VulkanPhysicalDevice.h"

namespace quoll::rhi {

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

  /**
   * @brief Set object name
   *
   * @param name Object name
   * @param type Obejct type
   * @param handle Object handle
   */
  void setObjectName(const String &name, VkObjectType type, void *handle);

private:
  VkDevice mDevice = VK_NULL_HANDLE;
  const VulkanPhysicalDevice &mPhysicalDevice;
};

} // namespace quoll::rhi
