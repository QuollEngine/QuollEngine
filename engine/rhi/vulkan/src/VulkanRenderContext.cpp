#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanCommandBuffer.h"
#include "VulkanRenderContext.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanRenderContext::VulkanRenderContext(VulkanDeviceObject &device,
                                         VulkanCommandPool &pool,
                                         VulkanQueue &graphicsQueue,
                                         VulkanQueue &presentQueue)
    : mDevice(device), mGraphicsQueue(graphicsQueue),
      mPresentQueue(presentQueue) {

  mRenderCommandLists =
      std::move(pool.createCommandLists(RenderDevice::NumFrames));
}

VkResult VulkanRenderContext::present(VulkanFrameManager &frameManager,
                                      const VulkanSwapchain &swapchain,
                                      uint32_t imageIdx) {
  LIQUID_PROFILE_EVENT("VulkanRenderContext::present");
  std::array<VkSemaphore, 1> waitSemaphores{
      frameManager.getRenderFinishedSemaphore()};
  std::array<VkSwapchainKHR, 1> swapchains{swapchain};
  VkPresentInfoKHR presentInfo{};

  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  presentInfo.pWaitSemaphores = waitSemaphores.data();
  presentInfo.swapchainCount = static_cast<uint32_t>(swapchains.size());
  presentInfo.pSwapchains = swapchains.data();
  presentInfo.pImageIndices = &imageIdx;
  presentInfo.pResults = nullptr;

  return vkQueuePresentKHR(mPresentQueue, &presentInfo);
}

RenderCommandList &
VulkanRenderContext::beginRendering(VulkanFrameManager &frameManager) {
  auto *commandBuffer =
      dynamic_cast<rhi::VulkanCommandBuffer *>(
          mRenderCommandLists.at(frameManager.getCurrentFrameIndex())
              .getNativeRenderCommandList()
              .get())
          ->getVulkanCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  checkForVulkanError(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                      "Failed to begin recording command buffer");

  return mRenderCommandLists.at(frameManager.getCurrentFrameIndex());
}

void VulkanRenderContext::endRendering(VulkanFrameManager &frameManager) {
  auto *commandBuffer =
      dynamic_cast<rhi::VulkanCommandBuffer *>(
          mRenderCommandLists.at(frameManager.getCurrentFrameIndex())
              .getNativeRenderCommandList()
              .get())
          ->getVulkanCommandBuffer();

  vkEndCommandBuffer(commandBuffer);

  VkCommandBufferSubmitInfo commandBufferInfo{};
  commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  commandBufferInfo.pNext = nullptr;
  commandBufferInfo.commandBuffer = commandBuffer;
  commandBufferInfo.deviceMask = 0;

  VkSemaphoreSubmitInfo waitSemaphoreInfo{};
  waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  waitSemaphoreInfo.pNext = nullptr;
  waitSemaphoreInfo.stageMask = 0;
  waitSemaphoreInfo.semaphore = frameManager.getImageAvailableSemaphore();
  waitSemaphoreInfo.deviceIndex = 0;
  waitSemaphoreInfo.value = 0;

  VkSemaphoreSubmitInfo signalSemaphoreInfo{};
  signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  signalSemaphoreInfo.pNext = nullptr;
  signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  signalSemaphoreInfo.semaphore = frameManager.getRenderFinishedSemaphore();
  signalSemaphoreInfo.deviceIndex = 0;
  signalSemaphoreInfo.value = 0;

  std::array<VkCommandBufferSubmitInfo, 1> commandBufferInfos{
      commandBufferInfo};
  std::array<VkSemaphoreSubmitInfo, 1> waitSemaphoreInfos{waitSemaphoreInfo};
  std::array<VkSemaphoreSubmitInfo, 1> signalSemaphoreInfos{
      signalSemaphoreInfo};

  mGraphicsQueue.submit(frameManager.getFrameFence(), commandBufferInfos,
                        waitSemaphoreInfos, signalSemaphoreInfos);
}

} // namespace liquid::rhi
