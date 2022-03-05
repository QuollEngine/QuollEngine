#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "VulkanUploadContext.h"
#include "VulkanError.h"

namespace liquid {

VulkanUploadContext::VulkanUploadContext(
    experimental::VulkanRenderDevice *device_) {
  device = device_->getVulkanDevice();
  graphicsQueue = device_->getGraphicsQueue();

  createCommandPool(device_->getPhysicalDevice()
                        .getQueueFamilyIndices()
                        .graphicsFamily.value());
  createFence();
}

VulkanUploadContext::~VulkanUploadContext() {
  if (uploadFence) {
    vkDestroyFence(device, uploadFence, nullptr);
    uploadFence = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Upload fence destroyed");
  }

  if (uploadCommandPool) {
    vkDestroyCommandPool(device, uploadCommandPool, nullptr);
    uploadCommandPool = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Upload command pool destroyed");
  }
}

void VulkanUploadContext::submit(const SubmitFn &submitFn) const {
  if (!device)
    return;

  VkCommandBuffer commandBuffer = nullptr;

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.pNext = nullptr;
  allocInfo.commandPool = uploadCommandPool;
  allocInfo.commandBufferCount = 1;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  checkForVulkanError(
      vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer),
      "Failed to allocate command buffer for uploads");

  vkWaitForFences(device, 1, &uploadFence, true,
                  std::numeric_limits<uint32_t>::max());

  vkResetFences(device, 1, &uploadFence);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  checkForVulkanError(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                      "Failed to start recording command buffer for uploads");

  submitFn(commandBuffer);

  checkForVulkanError(vkEndCommandBuffer(commandBuffer),
                      "Failed to stop recording command buffer");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, uploadFence);

  vkWaitForFences(device, 1, &uploadFence, true,
                  std::numeric_limits<uint32_t>::max());
  vkResetCommandPool(device, uploadCommandPool, 0);
}

void VulkanUploadContext::createCommandPool(uint32_t graphicsFamily) {
  VkCommandPoolCreateInfo uploadPoolInfo{};
  uploadPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  uploadPoolInfo.flags = 0;
  uploadPoolInfo.pNext = nullptr;

  uploadPoolInfo.queueFamilyIndex = graphicsFamily;

  checkForVulkanError(
      vkCreateCommandPool(device, &uploadPoolInfo, nullptr, &uploadCommandPool),
      "Failed to create upload command pool");

  LOG_DEBUG("[Vulkan] Upload command pool created");
}

void VulkanUploadContext::createFence() {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  checkForVulkanError(vkCreateFence(device, &fenceInfo, nullptr, &uploadFence),
                      "Failed to create upload fence");

  LOG_DEBUG("[Vulkan] Upload fence created");
}

} // namespace liquid
