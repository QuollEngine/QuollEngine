#pragma once

#include "VulkanDeviceObject.h"

namespace quoll::rhi {

class VulkanQueue {
public:
  VulkanQueue(VulkanDeviceObject &device, u32 queueIndex);

  inline u32 getQueueIndex() const { return mQueueIndex; }

  inline operator VkQueue() const { return mQueue; }

  inline VkQueue getVulkanHandle() const { return mQueue; }

  void submit(VkFence fence,
              std::span<VkCommandBufferSubmitInfo> commandBufferInfos,
              std::span<VkSemaphoreSubmitInfo> waitSemaphoreInfos,
              std::span<VkSemaphoreSubmitInfo> signalSemaphoreInfos);

  void waitForIdle();

private:
  VkQueue mQueue = VK_NULL_HANDLE;
  u32 mQueueIndex = 0;
};

} // namespace quoll::rhi
