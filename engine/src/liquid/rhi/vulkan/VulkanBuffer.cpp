#include "liquid/core/Base.h"

#include "VulkanBuffer.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanBuffer::VulkanBuffer(const BufferDescription &description,
                           VulkanResourceAllocator &allocator)
    : mAllocator(allocator), mType(description.type), mSize(description.size) {
  createBuffer(description);
}

VulkanBuffer::~VulkanBuffer() { destroyBuffer(); }

void VulkanBuffer::update(const BufferDescription &description) {
  LIQUID_ASSERT(mType == description.type,
                "Cannot change the type of the buffer");

  if (mSize != description.size) {
    destroyBuffer();
    createBuffer(description);
  } else if (description.data) {
    void *data = nullptr;
    vmaMapMemory(mAllocator, mAllocation, &data);
    memcpy(data, description.data, description.size);
    vmaUnmapMemory(mAllocator, mAllocation);
  }
}

void VulkanBuffer::createBuffer(const BufferDescription &description) {
  mSize = description.size;
  mType = description.type;

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
  } else if (description.type == rhi::BufferType::Transfer) {
    bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
  }

  VkBufferCreateInfo createBufferInfo{};
  createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  createBufferInfo.pNext = nullptr;
  createBufferInfo.flags = 0;
  createBufferInfo.size = description.size;
  createBufferInfo.usage = bufferUsage;

  VmaAllocationCreateInfo createAllocationInfo{};
  createAllocationInfo.flags = 0;
  createAllocationInfo.usage = memoryUsage;

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
}

void VulkanBuffer::destroyBuffer() {
  vmaDestroyBuffer(mAllocator, mBuffer, mAllocation);
}

} // namespace liquid::rhi
