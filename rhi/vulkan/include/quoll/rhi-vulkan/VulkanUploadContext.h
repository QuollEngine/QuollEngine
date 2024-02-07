#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"

namespace quoll::rhi {

class VulkanUploadContext : NoCopyMove {
  using SubmitFn = std::function<void(VkCommandBuffer)>;

public:
  VulkanUploadContext(VulkanDeviceObject &device, VulkanCommandPool &pool,
                      VulkanQueue &queue);

  ~VulkanUploadContext();

  void submit(const SubmitFn &submitFn) const;

private:
  void createFence();

private:
  VkFence mUploadFence = VK_NULL_HANDLE;
  RenderCommandList mCommandList;
  VulkanQueue &mQueue;
  VulkanDeviceObject &mDevice;
};

} // namespace quoll::rhi
