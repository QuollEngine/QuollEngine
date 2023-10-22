#pragma once

#include "quoll/rhi/RenderCommandList.h"
#include "quoll/rhi/DeviceStats.h"

#include "VulkanDeviceObject.h"
#include "VulkanDescriptorPool.h"

namespace quoll::rhi {

/**
 * @brief Vulkan command pool
 */
class VulkanCommandPool {
public:
  /**
   * @brief Create command pool
   *
   * @param device Render device
   * @param queueFamilyIndex Queue family index
   * @param registry Vulkan registry
   * @param descriptorPool Descriptor pool
   * @param stats Device stats
   */
  VulkanCommandPool(VulkanDeviceObject &device, u32 queueFamilyIndex,
                    const VulkanResourceRegistry &registry,
                    const VulkanDescriptorPool &descriptorPool,
                    DeviceStats &stats);

  /**
   * @brief Destroy command pool
   */
  ~VulkanCommandPool();

  VulkanCommandPool(const VulkanCommandPool &) = delete;
  VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;
  VulkanCommandPool(VulkanCommandPool &&) = delete;
  VulkanCommandPool &operator=(VulkanCommandPool &&) = delete;

  /**
   * @brief Create command buffers
   *
   * @param count Number of buffers
   * @return List of render command lists
   */
  std::vector<RenderCommandList> createCommandLists(u32 count);

  /**
   * @brief Free command list
   *
   * @param commandList Command list
   */
  void freeCommandList(RenderCommandList &commandList);

private:
  VkCommandPool mCommandPool = VK_NULL_HANDLE;
  VulkanDeviceObject &mDevice;
  DeviceStats &mStats;
  const VulkanResourceRegistry &mRegistry;
  const VulkanDescriptorPool &mDescriptorPool;
  u32 mQueueFamilyIndex = 0;
};

} // namespace quoll::rhi
