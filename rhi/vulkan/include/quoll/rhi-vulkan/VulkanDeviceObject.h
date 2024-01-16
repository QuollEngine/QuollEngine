#pragma once

#include "VulkanHeaders.h"
#include "VulkanPhysicalDevice.h"

namespace quoll::rhi {

class VulkanDeviceObject {
public:
  VulkanDeviceObject(const VulkanPhysicalDevice &physicalDevice);

  ~VulkanDeviceObject();

  VulkanDeviceObject(const VulkanDeviceObject &) = delete;
  VulkanDeviceObject &operator=(const VulkanDeviceObject &) = delete;
  VulkanDeviceObject(VulkanDeviceObject &&) = delete;
  VulkanDeviceObject &operator=(VulkanDeviceObject &&) = delete;

  inline operator VkDevice() const { return mDevice; }

  inline VkDevice getVulkanHandle() const { return mDevice; }

  void setObjectName(const String &name, VkObjectType type, void *handle);

private:
  VkDevice mDevice = VK_NULL_HANDLE;
  const VulkanPhysicalDevice &mPhysicalDevice;
};

} // namespace quoll::rhi
