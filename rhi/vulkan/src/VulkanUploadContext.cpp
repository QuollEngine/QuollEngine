#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "VulkanCommandBuffer.h"
#include "VulkanError.h"
#include "VulkanLog.h"
#include "VulkanUploadContext.h"

namespace quoll::rhi {

VulkanUploadContext::VulkanUploadContext(VulkanDeviceObject &mDevice,
                                         VulkanCommandPool &pool,
                                         VulkanQueue &queue)
    : mDevice(mDevice), mQueue(queue) {

  mCommandList = std::move(pool.createCommandLists(1).at(0));

  createFence();
}

VulkanUploadContext::~VulkanUploadContext() {
  if (mUploadFence) {
    vkDestroyFence(mDevice, mUploadFence, nullptr);
    LOG_DEBUG_VK("Upload fence destroyed", mUploadFence);
  }
}

void VulkanUploadContext::submit(const SubmitFn &submitFn) const {
  vkWaitForFences(mDevice, 1, &mUploadFence, true,
                  std::numeric_limits<u32>::max());

  vkResetFences(mDevice, 1, &mUploadFence);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  auto *commandBuffer = dynamic_cast<rhi::VulkanCommandBuffer *>(
                            mCommandList.getNativeRenderCommandList().get())
                            ->getVulkanCommandBuffer();

  checkForVulkanError(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                      "Failed to start recording command buffer for uploads");

  submitFn(commandBuffer);

  checkForVulkanError(vkEndCommandBuffer(commandBuffer),
                      "Failed to stop recording command buffer");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(mQueue, 1, &submitInfo, mUploadFence);

  vkWaitForFences(mDevice, 1, &mUploadFence, true,
                  std::numeric_limits<u32>::max());

  vkResetCommandBuffer(commandBuffer, 0);
}

void VulkanUploadContext::createFence() {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  checkForVulkanError(
      vkCreateFence(mDevice, &fenceInfo, nullptr, &mUploadFence),
      "Failed to create upload fence");

  LOG_DEBUG_VK("Upload fence created", mUploadFence);
}

} // namespace quoll::rhi
