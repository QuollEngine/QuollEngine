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

  VmaVulkanFunctions vulkanFns{};
  vulkanFns.vkAllocateMemory = vkAllocateMemory;
  vulkanFns.vkBindBufferMemory = vkBindBufferMemory;
  vulkanFns.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
  vulkanFns.vkBindImageMemory = vkBindImageMemory;
  vulkanFns.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
  vulkanFns.vkCmdCopyBuffer = vkCmdCopyBuffer;
  vulkanFns.vkCreateBuffer = vkCreateBuffer;
  vulkanFns.vkCreateImage = vkCreateImage;
  vulkanFns.vkDestroyBuffer = vkDestroyBuffer;
  vulkanFns.vkDestroyImage = vkDestroyImage;
  vulkanFns.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
  vulkanFns.vkFreeMemory = vkFreeMemory;
  vulkanFns.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
  vulkanFns.vkGetBufferMemoryRequirements2KHR =
      vkGetBufferMemoryRequirements2KHR;
  vulkanFns.vkGetDeviceBufferMemoryRequirements =
      vkGetDeviceBufferMemoryRequirements;
  vulkanFns.vkGetDeviceImageMemoryRequirements =
      vkGetDeviceImageMemoryRequirements;
  vulkanFns.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
  vulkanFns.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
  vulkanFns.vkMapMemory = vkMapMemory;
  vulkanFns.vkUnmapMemory = vkUnmapMemory;

  vulkanFns.vkGetPhysicalDeviceMemoryProperties =
      vkGetPhysicalDeviceMemoryProperties;
  vulkanFns.vkGetPhysicalDeviceMemoryProperties2KHR =
      vkGetPhysicalDeviceMemoryProperties2KHR;
  vulkanFns.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;

  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = backend.getVulkanInstance();
  createInfo.physicalDevice = physicalDevice;
  createInfo.device = device;
  createInfo.pVulkanFunctions = &vulkanFns;

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
