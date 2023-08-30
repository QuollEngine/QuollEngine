#include "liquid/core/Base.h"
#include "VulkanQueue.h"

#include "VulkanError.h"

namespace quoll::rhi {

VulkanQueue::VulkanQueue(VulkanDeviceObject &device, uint32_t queueIndex)
    : mQueueIndex(queueIndex) {
  vkGetDeviceQueue(device, mQueueIndex, 0, &mQueue);
}

void VulkanQueue::submit(
    VkFence fence, std::span<VkCommandBufferSubmitInfo> commandBufferInfos,
    std::span<VkSemaphoreSubmitInfo> waitSemaphoreInfos,
    std::span<VkSemaphoreSubmitInfo> signalSemaphoreInfos) {

  VkSubmitInfo2 submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  submitInfo.pNext = nullptr;
  submitInfo.flags = 0;
  submitInfo.commandBufferInfoCount =
      static_cast<uint32_t>(commandBufferInfos.size());
  submitInfo.pCommandBufferInfos = commandBufferInfos.data();
  submitInfo.waitSemaphoreInfoCount =
      static_cast<uint32_t>(waitSemaphoreInfos.size());
  submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfos.data();
  submitInfo.signalSemaphoreInfoCount =
      static_cast<uint32_t>(signalSemaphoreInfos.size());
  submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfos.data();

  checkForVulkanError(vkQueueSubmit2KHR(mQueue, 1, &submitInfo, fence),
                      "Failed to submit to queue");
}

void VulkanQueue::waitForIdle() { vkQueueWaitIdle(mQueue); }

} // namespace quoll::rhi
