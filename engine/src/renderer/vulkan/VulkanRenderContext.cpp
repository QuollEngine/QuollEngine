#include "VulkanRenderContext.h"
#include "core/EngineGlobals.h"
#include "VulkanError.h"

namespace liquid {

void VulkanRenderContext::create(const VulkanContext &context) {
  device = context.getDevice();
  createCommandBuffers(context.getPhysicalDevice()
                           .getQueueFamilyIndices()
                           .graphicsFamily.value());

  createSemaphores();
  createFences();

  graphicsQueue = context.getGraphicsQueue();
  presentQueue = context.getPresentQueue();
}

void VulkanRenderContext::destroy() {
  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    if (renderFences.at(i)) {
      vkDestroyFence(device, renderFences.at(i), nullptr);
      renderFences.at(i) = nullptr;
    }
  }
  LOG_DEBUG("[Vulkan] Render fences destroyed");

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    if (imageAvailableSemaphores.at(i)) {
      vkDestroySemaphore(device, imageAvailableSemaphores.at(i), nullptr);
      imageAvailableSemaphores.at(i) = nullptr;
    }

    if (renderFinishedSemaphores.at(i)) {
      vkDestroySemaphore(device, renderFinishedSemaphores.at(i), nullptr);
      renderFinishedSemaphores.at(i) = nullptr;
    }
  }
  LOG_DEBUG("[Vulkan] Render semaphores destroyed");

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    delete commandExecutors.at(i);
  }

  vkDestroyCommandPool(device, commandPool, nullptr);
  LOG_DEBUG("[Vulkan] Command Pool destroyed");
}

void VulkanRenderContext::render(RenderCommandList &commandList) {
  auto *executor = beginRendering();

  executor->execute(commandList);

  endRendering();
}

VkResult VulkanRenderContext::present(const VulkanSwapchain &swapchain,
                                      uint32_t imageIdx) {
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

  return vkQueuePresentKHR(presentQueue, &presentInfo);
}

VulkanCommandExecutor *VulkanRenderContext::beginRendering() {
  vkWaitForFences(device, 1, &renderFences.at(currentFrame), true,
                  std::numeric_limits<uint32_t>::max());
  vkResetFences(device, 1, &renderFences.at(currentFrame));

  return commandExecutors.at(currentFrame);
}

void VulkanRenderContext::endRendering() {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  std::array<VkCommandBuffer, 1> commandBuffers{
      commandExecutors.at(currentFrame)->getCommandBuffer()};

  std::array<VkSemaphore, 1> waitSemaphores{
      imageAvailableSemaphores.at(currentFrame)};
  std::array<VkPipelineStageFlags, 1> waitStages{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = waitSemaphores.size();
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.pWaitDstStageMask = waitStages.data();
  submitInfo.commandBufferCount = commandBuffers.size();
  submitInfo.pCommandBuffers = commandBuffers.data();

  std::array<VkSemaphore, 1> signalSemaphores{
      renderFinishedSemaphores.at(currentFrame)};
  submitInfo.signalSemaphoreCount = signalSemaphores.size();
  submitInfo.pSignalSemaphores = signalSemaphores.data();

  checkForVulkanError(vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                                    renderFences.at(currentFrame)),
                      "Failed to submit graphics queue");
}

void VulkanRenderContext::createSemaphores() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = nullptr;
  semaphoreInfo.flags = 0;

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    checkForVulkanError(vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                          &imageAvailableSemaphores.at(i)),
                        "Failed to create image available semaphore");

    checkForVulkanError(vkCreateSemaphore(device, &semaphoreInfo, nullptr,
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
        vkCreateFence(device, &fenceInfo, nullptr, &renderFences.at(i)),
        "Failed to create render fence");
  }

  LOG_DEBUG("[Vulkan] Render fence created");
}

void VulkanRenderContext::createCommandBuffers(uint32_t queueFamily) {
  std::array<VkCommandBuffer, NUM_FRAMES> commandBuffers{};

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamily;

  checkForVulkanError(
      vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
      "Failed to create command pool");

  LOG_DEBUG("[Vulkan] Command pool created");

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = NUM_FRAMES;

  checkForVulkanError(
      vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()),
      "Failed to allocate command buffers");

  for (size_t i = 0; i < NUM_FRAMES; ++i) {
    commandExecutors.at(i) = new VulkanCommandExecutor(commandBuffers.at(i));
  }

  LOG_DEBUG("[Vulkan] Command buffers allocated");
}

} // namespace liquid
