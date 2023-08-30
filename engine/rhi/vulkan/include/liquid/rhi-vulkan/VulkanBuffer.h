#pragma once

#include "VulkanHeaders.h"

#include "VulkanResourceAllocator.h"
#include "liquid/rhi/NativeBuffer.h"
#include "liquid/rhi/BufferDescription.h"

namespace quoll::rhi {

/**
 * @brief Vulkan hardware buffer
 */
class VulkanBuffer : public NativeBuffer {
public:
  /**
   * @brief Create buffer
   *
   * @param description Buffer description
   * @param allocator Vulkan allocator
   * @param device Vulkan device object
   */
  VulkanBuffer(const BufferDescription &description,
               VulkanResourceAllocator &allocator, VulkanDeviceObject &device);

  /**
   * @brief Destroy buffer
   */
  ~VulkanBuffer();

  VulkanBuffer(const VulkanBuffer &) = delete;
  VulkanBuffer &operator=(const VulkanBuffer &) = delete;
  VulkanBuffer(VulkanBuffer &&) = delete;
  VulkanBuffer &operator=(VulkanBuffer &&) = delete;

  /**
   * @brief Map buffer
   *
   * @return Mapped data
   */
  void *map() override;

  /**
   * @brief Unmap buffer
   */
  void unmap() override;

  /**
   * @brief Resize buffer
   *
   * Recreates the buffer with
   * new size. Does not retain
   * the previous data in it
   *
   * @param size New size
   */
  void resize(size_t size) override;

  /**
   * @brief Get device address
   *
   * @return Buffer device address
   */
  DeviceAddress getAddress() override;

  /**
   * @brief Get Vulkan buffer
   *
   * @return Vulkan buffer
   */
  inline VkBuffer getBuffer() const { return mBuffer; }

  /**
   * @brief Get Vulkan allocation
   *
   * @return Vulkan allocation
   */
  inline VmaAllocation getAllocation() const { return mAllocation; }

  /**
   * @brief Get buffer usage
   *
   * @return Buffer usage
   */
  inline rhi::BufferUsage getUsage() const { return mUsage; }

  /**
   * @brief Get buffer size
   *
   * @return Buffer size
   */
  inline size_t getSize() const { return mSize; }

private:
  /**
   * @brief Create buffer
   *
   * @param description Buffer description
   */
  void createBuffer(const BufferDescription &description);

private:
  VulkanResourceAllocator &mAllocator;
  VulkanDeviceObject &mDevice;

  VkBuffer mBuffer = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  rhi::BufferUsage mUsage;
  rhi::BufferAllocationUsage mAllocationUsage;
  bool mMapped = false;
  size_t mSize = 0;
  void *mMappedData = nullptr;

  String mDebugName;
};

} // namespace quoll::rhi
