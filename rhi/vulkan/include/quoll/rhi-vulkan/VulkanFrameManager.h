#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

/**
 * @brief Vulkan frame manager
 *
 * Stores synchronization primitives
 * for all frames in flight and advances
 * the frame
 */
class VulkanFrameManager {
public:
  VulkanFrameManager(VulkanDeviceObject &device);

  ~VulkanFrameManager();

  VulkanFrameManager(const VulkanFrameManager &) = delete;
  VulkanFrameManager &operator=(const VulkanFrameManager &) = delete;
  VulkanFrameManager(VulkanFrameManager &&) = delete;
  VulkanFrameManager &operator=(VulkanFrameManager &&) = delete;

  inline VkFence getFrameFence() const { return mFrameFences.at(mFrameIndex); }

  inline VkSemaphore getImageAvailableSemaphore() const {
    return mImageAvailableSemaphores.at(mFrameIndex);
  }

  inline VkSemaphore getRenderFinishedSemaphore() const {
    return mRenderFinishedSemaphores.at(mFrameIndex);
  }

  inline u32 getCurrentFrameIndex() const { return mFrameIndex; }

  void nextFrame();

  void waitForFrame();

private:
  void createSemaphores();

  void createFences();

private:
  VulkanDeviceObject &mDevice;

  std::array<VkFence, RenderDevice::NumFrames> mFrameFences{};
  std::array<VkSemaphore, RenderDevice::NumFrames> mImageAvailableSemaphores{};
  std::array<VkSemaphore, RenderDevice::NumFrames> mRenderFinishedSemaphores{};

  u32 mFrameIndex = 0;
};

} // namespace quoll::rhi
