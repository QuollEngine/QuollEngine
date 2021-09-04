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
   * @brief Constructor for vertex buffer
   *
   * @param allocator Vma Allocator
   * @param vertices List of vertices
   * @param statsManager Stats Manager
   */
  VulkanHardwareBuffer(VmaAllocator allocator,
                       const std::vector<Vertex> &vertices,
                       const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Constructor for index buffer
   *
   * @param allocator Vma Allocator
   * @param vertices List of vertices
   * @param statsManager Stats Manager
   */
  VulkanHardwareBuffer(VmaAllocator allocator,
                       const std::vector<uint32_t> &indices,
                       const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Constructor for uniform buffer
   *
   * @param allocator Vma Allocator
   * @param bufferSize Buffer size
   * @param statsManager Stats Manager
   */
  VulkanHardwareBuffer(VmaAllocator allocator, size_t bufferSize,
                       const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Create hardware buffer
   *
   * @param type Buffer type
   * @param size Buffer size
   * @param allocator VmaAllocator
   * @param statsManager Stats manager
   */
  VulkanHardwareBuffer(HardwareBufferType type, size_t size,
                       VmaAllocator allocator,
                       const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Deletes Vulkan buffer
   */
  ~VulkanHardwareBuffer();

  VulkanHardwareBuffer(const VulkanHardwareBuffer &rhs) = delete;
  VulkanHardwareBuffer(VulkanHardwareBuffer &&rhs) = delete;
  VulkanHardwareBuffer &operator=(const VulkanHardwareBuffer &rhs) = delete;
  VulkanHardwareBuffer &operator=(VulkanHardwareBuffer &&rhs) = delete;

  void *map() override;

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
  /**
   * @brief Create vulkan buffer
   *
   * @param usage Buffer usage
   */
  void createBuffer(VkBufferUsageFlags usage);

private:
  VkBuffer buffer = nullptr;
  VmaAllocation allocation = nullptr;
  VmaAllocator allocator;
};

} // namespace liquid
