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

  VulkanSubmitInfo submitInfo{};
  submitInfo.commandBuffers = {commandBuffer};
  submitInfo.fence = frameManager.getFrameFence();
  submitInfo.signalSemaphores = {frameManager.getRenderFinishedSemaphore()};
  submitInfo.waitSemaphores = {frameManager.getImageAvailableSemaphore()};
  submitInfo.waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  mGraphicsQueue.submit(submitInfo);
}

} // namespace liquid::rhi
