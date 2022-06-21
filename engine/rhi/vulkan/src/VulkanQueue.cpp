#include "liquid/core/Base.h"
#include "VulkanQueue.h"

#include "VulkanError.h"

namespace liquid::rhi {

VulkanQueue::VulkanQueue(VulkanDeviceObject &device, uint32_t queueIndex)
    : mQueueIndex(queueIndex) {
  vkGetDeviceQueue(device, mQueueIndex, 0, &mQueue);
}

void VulkanQueue::submit(VulkanSubmitInfo submitInfo) {
  std::array<VkPipelineStageFlags, 1> waitStages{submitInfo.waitStages};
  VkSubmitInfo vkSubmitInfo{};
  vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  vkSubmitInfo.pNext = nullptr;
  vkSubmitInfo.waitSemaphoreCount =
      static_cast<uint32_t>(submitInfo.waitSemaphores.size());
  vkSubmitInfo.pWaitSemaphores = submitInfo.waitSemaphores.data();
  vkSubmitInfo.pWaitDstStageMask = waitStages.data();
  vkSubmitInfo.commandBufferCount =
      static_cast<uint32_t>(submitInfo.commandBuffers.size());
  vkSubmitInfo.pCommandBuffers = submitInfo.commandBuffers.data();
  vkSubmitInfo.signalSemaphoreCount =
      static_cast<uint32_t>(submitInfo.signalSemaphores.size());
  vkSubmitInfo.pSignalSemaphores = submitInfo.signalSemaphores.data();

  checkForVulkanError(vkQueueSubmit(mQueue, 1, &vkSubmitInfo, submitInfo.fence),
                      "Failed to submit to queue");
}

} // namespace liquid::rhi
