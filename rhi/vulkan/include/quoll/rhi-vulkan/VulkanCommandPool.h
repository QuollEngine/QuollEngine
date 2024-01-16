#pragma once

#include "quoll/rhi/DeviceStats.h"
#include "quoll/rhi/RenderCommandList.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

class VulkanCommandPool {
public:
  VulkanCommandPool(VulkanDeviceObject &device, u32 queueFamilyIndex,
                    const VulkanResourceRegistry &registry,
                    const VulkanDescriptorPool &descriptorPool,
                    DeviceStats &stats);

  ~VulkanCommandPool();

  VulkanCommandPool(const VulkanCommandPool &) = delete;
  VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;
  VulkanCommandPool(VulkanCommandPool &&) = delete;
  VulkanCommandPool &operator=(VulkanCommandPool &&) = delete;

  std::vector<RenderCommandList> createCommandLists(u32 count);

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
