#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanResourceAllocator.h"
#include "VulkanRenderDevice.h"
#include "VulkanError.h"
#include "VulkanLog.h"

namespace liquid::rhi {

VulkanResourceAllocator::VulkanResourceAllocator(
    VulkanRenderBackend &backend, VulkanPhysicalDevice &physicalDevice,
    VulkanDeviceObject &device) {

  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = backend.getVulkanInstance();
  createInfo.physicalDevice = physicalDevice;
  createInfo.device = device;

  checkForVulkanError(vmaCreateAllocator(&createInfo, &mAllocator),
                      "Failed to create VMA allocator");

  LOG_DEBUG_VK("Resource allocator created", mAllocator);
}

VulkanResourceAllocator::~VulkanResourceAllocator() {
  if (mAllocator) {
    vmaDestroyAllocator(mAllocator);
    LOG_DEBUG_VK("Resource allocator destroyed", mAllocator);
  }
}

} // namespace liquid::rhi
