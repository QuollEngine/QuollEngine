#include "core/Base.h"
#include "core/EngineGlobals.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "scene/Vertex.h"

#include "VulkanHardwareBuffer.h"
#include "VulkanError.h"

namespace liquid {

VulkanHardwareBuffer::VulkanHardwareBuffer(HardwareBufferType type,
                                           size_t bufferSize, VkBuffer buffer_,
                                           VmaAllocation allocation_,
                                           VmaAllocator allocator_,
                                           StatsManager &statsManager)
    : HardwareBuffer(type, bufferSize, statsManager), buffer(buffer_),
      allocator(allocator_), allocation(allocation_) {}

void *VulkanHardwareBuffer::map() {
  void *buffer = nullptr;
  vmaMapMemory(allocator, allocation, &buffer);
  return buffer;
}

void VulkanHardwareBuffer::unmap() { vmaUnmapMemory(allocator, allocation); }

void VulkanHardwareBuffer::update(void *data) {
  void *buffer = map();
  memcpy(buffer, data, getBufferSize());
  unmap();
}

VulkanHardwareBuffer::~VulkanHardwareBuffer() {
  if (allocator && buffer && allocation) {
    vmaDestroyBuffer(allocator, buffer, allocation);
  }

  LOG_DEBUG("[Vulkan] Hardware buffer destroyed");
}

} // namespace liquid
