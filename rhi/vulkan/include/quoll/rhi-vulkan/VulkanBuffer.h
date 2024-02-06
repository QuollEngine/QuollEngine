#pragma once

#include "quoll/rhi/BufferDescription.h"
#include "quoll/rhi/NativeBuffer.h"
#include "VulkanHeaders.h"
#include "VulkanResourceAllocator.h"

namespace quoll::rhi {

class VulkanBuffer : public NativeBuffer, NoCopyMove {
public:
  VulkanBuffer(const BufferDescription &description,
               VulkanResourceAllocator &allocator, VulkanDeviceObject &device);

  ~VulkanBuffer();

  void *map() override;

  void unmap() override;

  void resize(usize size) override;

  DeviceAddress getAddress() override;

  inline VkBuffer getBuffer() const { return mBuffer; }

  inline VmaAllocation getAllocation() const { return mAllocation; }

  inline rhi::BufferUsage getUsage() const { return mUsage; }

  inline usize getSize() const { return mSize; }

private:
  void createBuffer(const BufferDescription &description);

private:
  VulkanResourceAllocator &mAllocator;
  VulkanDeviceObject &mDevice;

  VkBuffer mBuffer = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  rhi::BufferUsage mUsage;
  rhi::BufferAllocationUsage mAllocationUsage;
  bool mMapped = false;
  usize mSize = 0;
  void *mMappedData = nullptr;

  String mDebugName;
};

} // namespace quoll::rhi
