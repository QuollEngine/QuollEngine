#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanResourceAllocator.h"
#include "liquid/rhi/BufferDescription.h"

namespace liquid::rhi {

/**
 * @brief Vulkan hardware buffer
 */
class VulkanBuffer {
public:
  /**
   * @brief Create buffer
   *
   * @param description Buffer description
   * @param allocator Vulkan allocator
   */
  VulkanBuffer(const BufferDescription &description,
               VulkanResourceAllocator &allocator);

  /**
   * @brief Destroy buffer
   */
  ~VulkanBuffer();

  VulkanBuffer(const VulkanBuffer &) = delete;
  VulkanBuffer &operator=(const VulkanBuffer &) = delete;
  VulkanBuffer(VulkanBuffer &&) = delete;
  VulkanBuffer &operator=(VulkanBuffer &&) = delete;

  /**
   * @brief Update buffer
   *
   * @param description Buffer description
   */
  void update(const BufferDescription &description);

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
   * @brief Get buffer type
   *
   * @return Buffer type
   */
  inline rhi::BufferType getType() const { return mType; }

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

  /**
   * @brief Destroy buffer
   */
  void destroyBuffer();

private:
  VulkanResourceAllocator &mAllocator;

  VkBuffer mBuffer = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  rhi::BufferType mType;
  size_t mSize = 0;
};

} // namespace liquid::rhi
