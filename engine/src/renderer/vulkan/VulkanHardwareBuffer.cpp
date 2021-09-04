#include "core/Base.h"
#include "core/EngineGlobals.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "scene/Vertex.h"

#include "VulkanHardwareBuffer.h"
#include "VulkanError.h"

namespace liquid {

VulkanHardwareBuffer::VulkanHardwareBuffer(
    HardwareBufferType type, size_t size, VmaAllocator allocator_,
    const SharedPtr<StatsManager> &statsManager)
    : HardwareBuffer(type, size, statsManager), allocator(allocator_) {
  switch (getType()) {
  case HardwareBuffer::VERTEX:
    createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    break;
  case HardwareBuffer::INDEX:
    createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    break;
  case HardwareBuffer::UNIFORM:
    createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    break;
  }
}

VulkanHardwareBuffer::VulkanHardwareBuffer(
    VmaAllocator allocator_, const std::vector<Vertex> &vertices,
    const SharedPtr<StatsManager> &statsManager)
    : HardwareBuffer(vertices, statsManager), allocator(allocator_) {
  createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

  void *data = nullptr;
  vmaMapMemory(allocator, allocation, &data);
  memcpy(data, vertices.data(), getBufferSize());
  vmaUnmapMemory(allocator, allocation);

  LOG_DEBUG("[Vulkan] Vertex buffer created");
}

VulkanHardwareBuffer::VulkanHardwareBuffer(
    VmaAllocator allocator_, const std::vector<uint32_t> &indices,
    const SharedPtr<StatsManager> &statsManager)
    : HardwareBuffer(indices, statsManager), allocator(allocator_) {
  createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

  void *data = nullptr;
  vmaMapMemory(allocator, allocation, &data);
  memcpy(data, indices.data(), getBufferSize());
  vmaUnmapMemory(allocator, allocation);

  LOG_DEBUG("[Vulkan] Index buffer created");
}

VulkanHardwareBuffer::VulkanHardwareBuffer(
    VmaAllocator allocator_, size_t bufferSize,
    const SharedPtr<StatsManager> &statsManager)
    : HardwareBuffer(bufferSize, statsManager), allocator(allocator_) {
  createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

void *VulkanHardwareBuffer::map() {
  void *buffer = nullptr;
  vmaMapMemory(allocator, allocation, &buffer);
  return buffer;
}

void VulkanHardwareBuffer::unmap() { vmaUnmapMemory(allocator, allocation); }

void VulkanHardwareBuffer::update(void *data) {
  void *buffer = nullptr;
  vmaMapMemory(allocator, allocation, &buffer);
  memcpy(buffer, data, getBufferSize());
  vmaUnmapMemory(allocator, allocation);
}

void VulkanHardwareBuffer::createBuffer(VkBufferUsageFlags usage) {
  if (!allocator)
    return;

  VkBufferCreateInfo createBufferInfo{};
  createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  createBufferInfo.pNext = nullptr;
  createBufferInfo.flags = 0;
  createBufferInfo.size = getBufferSize();
  createBufferInfo.usage = usage;

  VmaAllocationCreateInfo createAllocationInfo{};
  createAllocationInfo.flags = 0;
  createAllocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  checkForVulkanError(vmaCreateBuffer(allocator, &createBufferInfo,
                                      &createAllocationInfo, &buffer,
                                      &allocation, nullptr),
                      "Cannot create hardware buffer");
}

VulkanHardwareBuffer::~VulkanHardwareBuffer() {
  if (allocator && buffer && allocation) {
    vmaDestroyBuffer(allocator, buffer, allocation);
  }

  LOG_DEBUG("[Vulkan] Hardware buffer destroyed");
}

} // namespace liquid
