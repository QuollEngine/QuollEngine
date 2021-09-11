#pragma once

#include "core/Base.h"

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "renderer/HardwareBuffer.h"

#include "scene/Vertex.h"

namespace liquid {

/**
 * @brief Creates Vulkan Hardware Buffer
 *
 * Uses VmaAllocator to create buffers
 */
class VulkanHardwareBuffer : public HardwareBuffer {
public:
  /**
   * @brief Create Vulkan hardware buffer
   *
   * @param type Buffer type
   * @param bufferSize Buffer size
   * @param buffer Vulkan buffer
   * @param allocation Vma allocation
   * @param bmaAllocator Vma allocator
   * @param statsManager Stats manager
   */
  VulkanHardwareBuffer(HardwareBufferType type, size_t bufferSize,
                       VkBuffer buffer, VmaAllocation allocation,
                       VmaAllocator allocator, StatsManager &statsManager);

  /**
   * @brief Deletes Vulkan buffer
   */
  ~VulkanHardwareBuffer();

  VulkanHardwareBuffer(const VulkanHardwareBuffer &rhs) = delete;
  VulkanHardwareBuffer(VulkanHardwareBuffer &&rhs) = delete;
  VulkanHardwareBuffer &operator=(const VulkanHardwareBuffer &rhs) = delete;
  VulkanHardwareBuffer &operator=(VulkanHardwareBuffer &&rhs) = delete;

  /**
   * @brief Map hardware buffer
   *
   * @return Mapped buffer data
   */
  void *map() override;

  /**
   * @brief Unmap hardware buffer
   */
  void unmap() override;

  /**
   * @brief Update buffer
   *
   * @param data Uniform data
   */
  void update(void *data) override;

  /**
   * @brief Get vulkan buffer handle
   *
   * @return Vulkan Buffer handle
   */
  inline VkBuffer getBuffer() { return buffer; }

  /**
   * @brief Get allocation
   *
   * @return Vma allocation
   */
  inline VmaAllocation getAllocation() { return allocation; }

private:
  VkBuffer buffer = nullptr;
  VmaAllocation allocation = nullptr;
  VmaAllocator allocator;
};

} // namespace liquid
