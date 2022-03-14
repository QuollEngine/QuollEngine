#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

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

  renderCommandLists = std::move(pool.createCommandLists(NUM_FRAMES));

  createSemaphores();
  createFences();
}

VulkanRenderContext::~VulkanRenderContext() {
  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    if (renderFences.at(i)) {
      vkDestroyFence(mDevice, renderFences.at(i), nullptr);
      renderFences.at(i) = VK_NULL_HANDLE;
    }
  }
  LOG_DEBUG("[Vulkan] Render fences destroyed");

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    if (imageAvailableSemaphores.at(i)) {
      vkDestroySemaphore(mDevice, imageAvailableSemaphores.at(i), nullptr);
      imageAvailableSemaphores.at(i) = VK_NULL_HANDLE;
    }

    if (renderFinishedSemaphores.at(i)) {
      vkDestroySemaphore(mDevice, renderFinishedSemaphores.at(i), nullptr);
      renderFinishedSemaphores.at(i) = VK_NULL_HANDLE;
    }
  }
  LOG_DEBUG("[Vulkan] Render semaphores destroyed");
}

VkResult VulkanRenderContext::present(const VulkanSwapchain &swapchain,
                                      uint32_t imageIdx) {
  LIQUID_PROFILE_EVENT("VulkanRenderContext::present");
  std::array<VkSemaphore, 1> waitSemaphores{
      renderFinishedSemaphores.at(currentFrame)};
  std::array<VkSwapchainKHR, 1> swapchains{swapchain.getSwapchain()};
  VkPresentInfoKHR presentInfo{};

  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  presentInfo.pWaitSemaphores = waitSemaphores.data();
  presentInfo.swapchainCount = static_cast<uint32_t>(swapchains.size());
  presentInfo.pSwapchains = swapchains.data();
  presentInfo.pImageIndices = &imageIdx;
  presentInfo.pResults = nullptr;

  currentFrame = (currentFrame + 1) % NUM_FRAMES;

  return vkQueuePresentKHR(mPresentQueue, &presentInfo);
}

RenderCommandList &VulkanRenderContext::beginRendering() {
  vkWaitForFences(mDevice, 1, &renderFences.at(currentFrame), true,
                  std::numeric_limits<uint32_t>::max());
  vkResetFences(mDevice, 1, &renderFences.at(currentFrame));

  auto *commandBuffer = dynamic_cast<rhi::VulkanCommandBuffer *>(
                            renderCommandLists.at(currentFrame)
                                .getNativeRenderCommandList()
                                .get())
                            ->getVulkanCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  checkForVulkanError(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                      "Failed to begin recording command buffer");

  return renderCommandLists.at(currentFrame);
}

void VulkanRenderContext::endRendering() {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  auto *commandBuffer = dynamic_cast<rhi::VulkanCommandBuffer *>(
                            renderCommandLists.at(currentFrame)
                                .getNativeRenderCommandList()
                                .get())
                            ->getVulkanCommandBuffer();

  vkEndCommandBuffer(commandBuffer);

  std::array<VkCommandBuffer, 1> commandBuffers{commandBuffer};

  std::array<VkSemaphore, 1> waitSemaphores{
      imageAvailableSemaphores.at(currentFrame)};
  std::array<VkPipelineStageFlags, 1> waitStages{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.pWaitDstStageMask = waitStages.data();
  submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
  submitInfo.pCommandBuffers = commandBuffers.data();

  std::array<VkSemaphore, 1> signalSemaphores{
      renderFinishedSemaphores.at(currentFrame)};
  submitInfo.signalSemaphoreCount =
      static_cast<uint32_t>(signalSemaphores.size());
  submitInfo.pSignalSemaphores = signalSemaphores.data();

  checkForVulkanError(vkQueueSubmit(mGraphicsQueue, 1, &submitInfo,
                                    renderFences.at(currentFrame)),
                      "Failed to submit graphics queue");
}

void VulkanRenderContext::createSemaphores() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = nullptr;
  semaphoreInfo.flags = 0;

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    checkForVulkanError(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                                          &imageAvailableSemaphores.at(i)),
                        "Failed to create image available semaphore");

    checkForVulkanError(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                                          &renderFinishedSemaphores.at(i)),
                        "Failed to create render finished semaphores");
  }

  LOG_DEBUG("[Vulkan] Render semaphores created");
}

void VulkanRenderContext::createFences() {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    checkForVulkanError(
        vkCreateFence(mDevice, &fenceInfo, nullptr, &renderFences.at(i)),
        "Failed to create render fence");
  }

  LOG_DEBUG("[Vulkan] Render fence created");
}

} // namespace liquid::rhi
