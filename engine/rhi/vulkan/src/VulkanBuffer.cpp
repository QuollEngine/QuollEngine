#include "liquid/core/Base.h"

#include "VulkanBuffer.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanBuffer::VulkanBuffer(const BufferDescription &description,
                           VulkanResourceAllocator &allocator,
                           VulkanDeviceObject &device)
    : mDevice(device), mAllocator(allocator), mUsage(description.usage),
      mAllocationUsage(description.allocationUsage), mSize(description.size),
      mMapped(description.mapped), mDebugName(description.debugName) {
  createBuffer(description);
}

VulkanBuffer::~VulkanBuffer() {
  vmaDestroyBuffer(mAllocator, mBuffer, mAllocation);
}

void *VulkanBuffer::map() {
  if (mMapped) {
    return mMappedData;
  }

  void *data = nullptr;
  vmaMapMemory(mAllocator, mAllocation, &data);
  return data;
}

void VulkanBuffer::unmap() {
  if (mMapped) {
    return;
  }

  vmaUnmapMemory(mAllocator, mAllocation);
}

void VulkanBuffer::resize(size_t size) {
  mSize = size;
  createBuffer({mUsage, mSize, nullptr, mAllocationUsage, mMapped, mDebugName});
}

DeviceAddress VulkanBuffer::getAddress() {
  VkBufferDeviceAddressInfo addressInfo{};
  addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  addressInfo.pNext = nullptr;
  addressInfo.buffer = mBuffer;

  return static_cast<DeviceAddress>(
      vkGetBufferDeviceAddress(mDevice, &addressInfo));
}

void VulkanBuffer::createBuffer(const BufferDescription &description) {
  LIQUID_ASSERT(description.size > 0,
                "Cannot create \"" + description.debugName +
                    "\" buffer because buffer size is zero.");

  mSize = description.size;
  mUsage = description.usage;
  mAllocationUsage = description.allocationUsage;

  VmaAllocationCreateFlags allocationFlags = 0;

  if (description.mapped) {
    allocationFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
  }

  if ((mAllocationUsage & BufferAllocationUsage::HostWrite) ==
      BufferAllocationUsage::HostWrite) {
    allocationFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
  }

  if ((mAllocationUsage & BufferAllocationUsage::HostRead) ==
      BufferAllocationUsage::HostRead) {
    allocationFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  }

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  if (BitwiseEnumContains(description.usage, rhi::BufferUsage::Vertex)) {
    bufferUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  } else if (BitwiseEnumContains(description.usage, rhi::BufferUsage::Index)) {
    bufferUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  } else if (BitwiseEnumContains(description.usage,
                                 rhi::BufferUsage::Uniform)) {
    bufferUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  } else if (BitwiseEnumContains(description.usage,
                                 rhi::BufferUsage::Storage)) {
    bufferUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  } else if (BitwiseEnumContains(description.usage,
                                 rhi::BufferUsage::TransferSource)) {
    bufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  } else if (BitwiseEnumContains(description.usage,
                                 rhi::BufferUsage::TransferDestination)) {
    bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  } else if (BitwiseEnumContains(description.usage,
                                 rhi::BufferUsage::Indirect)) {
    bufferUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  }

  LIQUID_ASSERT(bufferUsage != 0, "Buffer usage cannot be empty");

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

  VmaAllocationInfo allocationInfo{};

  checkForVulkanError(vmaCreateBuffer(mAllocator, &createBufferInfo,
                                      &createAllocationInfo, &mBuffer,
                                      &mAllocation, &allocationInfo),
                      "Cannot create buffer", description.debugName);

  mDevice.setObjectName(description.debugName, VK_OBJECT_TYPE_BUFFER, mBuffer);

  if (description.mapped) {
    mMappedData = allocationInfo.pMappedData;
  }

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
