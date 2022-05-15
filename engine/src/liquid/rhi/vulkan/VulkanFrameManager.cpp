#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "VulkanFrameManager.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanFrameManager::VulkanFrameManager(VulkanDeviceObject &device)
    : mDevice(device) {
  createFences();
  createSemaphores();
}

VulkanFrameManager::~VulkanFrameManager() {
  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    if (mFrameFences.at(i)) {
      vkDestroyFence(mDevice, mFrameFences.at(i), nullptr);
      mFrameFences.at(i) = VK_NULL_HANDLE;
    }
  }
  LOG_DEBUG("[Vulkan] Frame fences destroyed");

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    if (mImageAvailableSemaphores.at(i)) {
      vkDestroySemaphore(mDevice, mImageAvailableSemaphores.at(i), nullptr);
      mImageAvailableSemaphores.at(i) = VK_NULL_HANDLE;
    }

    if (mRenderFinishedSemaphores.at(i)) {
      vkDestroySemaphore(mDevice, mRenderFinishedSemaphores.at(i), nullptr);
      mRenderFinishedSemaphores.at(i) = VK_NULL_HANDLE;
    }
  }
  LOG_DEBUG("[Vulkan] Frame semaphores destroyed");
}

void VulkanFrameManager::nextFrame() {
  mFrameIndex = (mFrameIndex + 1) % NUM_FRAMES;
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

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    checkForVulkanError(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                                          &mImageAvailableSemaphores.at(i)),
                        "Failed to create image available semaphore");

    checkForVulkanError(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                                          &mRenderFinishedSemaphores.at(i)),
                        "Failed to create render finished semaphores");
  }

  LOG_DEBUG("[Vulkan] Render semaphores created");
}

void VulkanFrameManager::createFences() {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < NUM_FRAMES; ++i) {
    checkForVulkanError(
        vkCreateFence(mDevice, &fenceInfo, nullptr, &mFrameFences.at(i)),
        "Failed to create render fence");
  }

  LOG_DEBUG("[Vulkan] Render fence created");
}

} // namespace liquid::rhi
