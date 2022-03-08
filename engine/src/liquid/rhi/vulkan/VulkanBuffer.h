#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "../BufferDescription.h"

namespace liquid::experimental {

class VulkanBuffer {
public:
  /**
   * @brief Create buffer
   *
   * @param type Buffer type
   * @param size Buffer size
   * @param buffer Vulkan buffer
   * @param allocation Vulkan allocation
   */
  VulkanBuffer(BufferType type, size_t size, VkBuffer buffer,
               VmaAllocation allocation);

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
  inline BufferType getType() const { return mType; }

  /**
   * @brief Get buffer size
   *
   * @return Buffer size
   */
  inline size_t getSize() const { return mSize; }

private:
  VkBuffer mBuffer = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  BufferType mType;
  size_t mSize = 0;
};

} // namespace liquid::experimental
