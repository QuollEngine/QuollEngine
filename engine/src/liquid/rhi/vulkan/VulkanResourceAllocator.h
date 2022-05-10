#pragma once

#include "VulkanPhysicalDevice.h"
#include "VulkanRenderBackend.h"
#include "VulkanDeviceObject.h"

#include <vma/vk_mem_alloc.h>

namespace liquid::rhi {

/**
 * @brief Vulkan resource allocator
 *
 * Uses Vma allocator
 */
class VulkanResourceAllocator {
public:
  /**
   * @brief Create Vulkan reosurce allocator
   *
   * @param backend Vulkan render backend
   * @param physicalDevice Vulkan physical device
   * @param device Vulkan device
   */
  VulkanResourceAllocator(VulkanRenderBackend &backend,
                          VulkanPhysicalDevice &physicalDevice,
                          VulkanDeviceObject &device);

  /**
   * @brief Destroy Vulkan resource allocator
   */
  ~VulkanResourceAllocator();

  VulkanResourceAllocator(const VulkanResourceAllocator &) = delete;
  VulkanResourceAllocator &operator=(const VulkanResourceAllocator &) = delete;
  VulkanResourceAllocator(VulkanResourceAllocator &&) = delete;
  VulkanResourceAllocator &operator=(VulkanResourceAllocator &&) = delete;

  /**
   * @brief Get Vma allocator
   *
   * @return Vma allocator
   */
  inline operator VmaAllocator() { return mAllocator; }

private:
  VmaAllocator mAllocator = VK_NULL_HANDLE;
};

} // namespace liquid::rhi
