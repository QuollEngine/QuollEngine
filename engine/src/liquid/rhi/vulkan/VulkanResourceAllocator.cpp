#include "liquid/core/Base.h"

#include "VulkanResourceAllocator.h"
#include "VulkanRenderDevice.h"

#include "liquid/renderer/vulkan/VulkanError.h"
#include "liquid/core/EngineGlobals.h"

namespace liquid::experimental {

VulkanResourceAllocator::VulkanResourceAllocator(
    VulkanRenderBackend &backend, VulkanPhysicalDevice &physicalDevice,
    VulkanDeviceObject &device) {

  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = backend.getVulkanInstance();
  createInfo.physicalDevice = physicalDevice.getVulkanDevice();
  createInfo.device = device;

  checkForVulkanError(vmaCreateAllocator(&createInfo, &mAllocator),
                      "Failed to create VMA allocator");

  LOG_DEBUG("[Vulkan] Resource allocator created");
}

VulkanResourceAllocator::~VulkanResourceAllocator() {
  if (mAllocator) {
    vmaDestroyAllocator(mAllocator);
    LOG_DEBUG("[Vulkan] Resource allocator destroyed");
  }
}

} // namespace liquid::experimental
