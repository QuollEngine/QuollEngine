#pragma once

#include "quoll/rhi/RenderCommandList.h"
#include "VulkanCommandPool.h"
#include "VulkanDeviceObject.h"
#include "VulkanFrameManager.h"
#include "VulkanHeaders.h"
#include "VulkanQueue.h"
#include "VulkanSwapchain.h"
#include <vk_mem_alloc.h>

namespace quoll::rhi {

class VulkanRenderContext {
public:
  VulkanRenderContext(VulkanDeviceObject &device, VulkanCommandPool &pool,
                      VulkanQueue &graphicsQueue, VulkanQueue &presentQueue);

  VkResult present(VulkanFrameManager &frameManager,
                   const VulkanSwapchain &swapchain, u32 imageIdx);

  RenderCommandList &beginRendering(VulkanFrameManager &frameManager);

  void endRendering(VulkanFrameManager &frameManager);

private:
  std::vector<RenderCommandList> mRenderCommandLists;

  VulkanQueue &mGraphicsQueue;
  VulkanQueue &mPresentQueue;
  VulkanDeviceObject &mDevice;
};

} // namespace quoll::rhi
