#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanFrameManager.h"
#include "VulkanError.h"
#include "VulkanLog.h"

namespace liquid::rhi {

VulkanFrameManager::VulkanFrameManager(VulkanDeviceObject &device)
    : mDevice(device) {
  createFences();
  createSemaphores();
}

VulkanFrameManager::~VulkanFrameManager() {
  for (uint32_t i = 0; i < RenderDevice::NumFrames; ++i) {
    if (mFrameFences.at(i)) {
      vkDestroyFence(mDevice, mFrameFences.at(i), nullptr);
      mFrameFences.at(i) = VK_NULL_HANDLE;
    }
  }
  LOG_DEBUG_VK_NO_HANDLE("Frame fences destroyed: " << mFrameFences.size());

  for (uint32_t i = 0; i < RenderDevice::NumFrames; ++i) {
    if (mImageAvailableSemaphores.at(i)) {
      vkDestroySemaphore(mDevice, mImageAvailableSemaphores.at(i), nullptr);
      mImageAvailableSemaphores.at(i) = VK_NULL_HANDLE;
    }

    if (mRenderFinishedSemaphores.at(i)) {
      vkDestroySemaphore(mDevice, mRenderFinishedSemaphores.at(i), nullptr);
      mRenderFinishedSemaphores.at(i) = VK_NULL_HANDLE;
    }
  }
  LOG_DEBUG_VK_NO_HANDLE(
      "Frame semaphores destroyed: "
      << mImageAvailableSemaphores.size() << " image available; "
      << mRenderFinishedSemaphores.size() << " render finished");
}

void VulkanFrameManager::nextFrame() {
  mFrameIndex = (mFrameIndex + 1) % RenderDevice::NumFrames;
}

void VulkanFrameManager::waitForFrame() {
  vkWaitForFences(mDevice, 1, &mFrameFences.at(mFrameIndex), true,
                  std::numeric_limits<uint32_t>::max());
  vkResetFences(mDevice, 1, &mFrameFences.at(mFrameIndex));
}

void VulkanFrameManager::createSemaphores() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = nullptr;
  semaphoreInfo.flags = 0;

  for (uint32_t i = 0; i < RenderDevice::NumFrames; ++i) {
    checkForVulkanError(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                                          &mImageAvailableSemaphores.at(i)),
                        "Failed to create image available semaphore");

    checkForVulkanError(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                                          &mRenderFinishedSemaphores.at(i)),
                        "Failed to create render finished semaphores");
  }

  LOG_DEBUG_VK_NO_HANDLE(
      "Frame semaphores created: "
      << mImageAvailableSemaphores.size() << " image available; "
      << mRenderFinishedSemaphores.size() << " render finished");
}

void VulkanFrameManager::createFences() {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < RenderDevice::NumFrames; ++i) {
    checkForVulkanError(
        vkCreateFence(mDevice, &fenceInfo, nullptr, &mFrameFences.at(i)),
        "Failed to create render fence");
  }

  LOG_DEBUG_VK_NO_HANDLE("Frame fences created: " << mFrameFences.size());
}

} // namespace liquid::rhi
