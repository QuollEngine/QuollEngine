#pragma once

#include "VulkanHeaders.h"
#include "VulkanPhysicalDevice.h"

namespace quoll::rhi {

class VulkanDeviceObject : NoCopyMove {
public:
  VulkanDeviceObject(const VulkanPhysicalDevice &physicalDevice);

  ~VulkanDeviceObject();

  inline operator VkDevice() const { return mDevice; }

  inline VkDevice getVulkanHandle() const { return mDevice; }

  void setObjectName(const String &name, VkObjectType type, void *handle);

private:
  VkDevice mDevice = VK_NULL_HANDLE;
  const VulkanPhysicalDevice &mPhysicalDevice;
};

} // namespace quoll::rhi
