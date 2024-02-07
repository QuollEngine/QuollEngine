#pragma once

#include "VulkanDeviceObject.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanRenderBackend.h"
#include <vk_mem_alloc.h>

namespace quoll::rhi {

class VulkanResourceAllocator : NoCopyMove {
public:
  VulkanResourceAllocator(VulkanRenderBackend &backend,
                          VulkanPhysicalDevice &physicalDevice,
                          VulkanDeviceObject &device);

  ~VulkanResourceAllocator();

  inline operator VmaAllocator() { return mAllocator; }

private:
  VmaAllocator mAllocator = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
