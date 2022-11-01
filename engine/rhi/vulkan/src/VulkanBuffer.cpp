#include "liquid/core/Base.h"

#include "VulkanBuffer.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanBuffer::VulkanBuffer(const BufferDescription &description,
                           VulkanResourceAllocator &allocator)
    : mAllocator(allocator), mType(description.type), mUsage(description.usage),
      mSize(description.size) {
  createBuffer(description);
}

VulkanBuffer::~VulkanBuffer() {
  vmaDestroyBuffer(mAllocator, mBuffer, mAllocation);
}

void *VulkanBuffer::map() {
  void *data = nullptr;
  vmaMapMemory(mAllocator, mAllocation, &data);
  return data;
}

void VulkanBuffer::unmap() { vmaUnmapMemory(mAllocator, mAllocation); }

void VulkanBuffer::resize(size_t size) {
  mSize = size;
  createBuffer({mType, mSize, nullptr, mUsage});
}

void VulkanBuffer::createBuffer(const BufferDescription &description) {
  mSize = description.size;
  mType = description.type;
  mUsage = description.usage;

  VmaAllocationCreateFlags allocationFlags = 0;

  if ((mUsage & BufferUsage::HostWrite) == BufferUsage::HostWrite) {
    allocationFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  }

  if ((mUsage & BufferUsage::HostRead) == BufferUsage::HostRead) {
    allocationFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  }

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  if (description.type == rhi::BufferType::Vertex) {
    bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  } else if (description.type == rhi::BufferType::Index) {
    bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  } else if (description.type == rhi::BufferType::Uniform) {
    bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  } else if (description.type == rhi::BufferType::Storage) {
    bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  } else if (description.type == rhi::BufferType::TransferSource) {
    bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  } else if (description.type == rhi::BufferType::TransferDestination) {
    bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }

  VkBufferCreateInfo createBufferInfo{};
  createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  createBufferInfo.pNext = nullptr;
  createBufferInfo.flags = 0;
  createBufferInfo.size = description.size;
  createBufferInfo.usage = bufferUsage;

  VmaAllocationCreateInfo createAllocationInfo{};
  createAllocationInfo.flags = allocationFlags;
  createAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

  VmaAllocation oldAllocation = mAllocation;
  VkBuffer oldBuffer = mBuffer;

  checkForVulkanError(vmaCreateBuffer(mAllocator, &createBufferInfo,
                                      &createAllocationInfo, &mBuffer,
                                      &mAllocation, nullptr),
                      "Cannot create buffer");

  if (description.data) {
    void *data = nullptr;
    vmaMapMemory(mAllocator, mAllocation, &data);
    memcpy(data, description.data, description.size);
    vmaUnmapMemory(mAllocator, mAllocation);
  }

  if (oldAllocation && oldBuffer) {
    vmaDestroyBuffer(mAllocator, oldBuffer, oldAllocation);
  }
}

} // namespace liquid::rhi
