#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"

namespace quoll::rhi {

class VulkanUploadContext {
  using SubmitFn = std::function<void(VkCommandBuffer)>;

public:
  VulkanUploadContext(VulkanDeviceObject &device, VulkanCommandPool &pool,
                      VulkanQueue &queue);

  ~VulkanUploadContext();

  VulkanUploadContext(const VulkanUploadContext &) = delete;
  VulkanUploadContext(VulkanUploadContext &&) = delete;
  VulkanUploadContext &operator=(const VulkanUploadContext &) = delete;
  VulkanUploadContext &operator=(VulkanUploadContext &&) = delete;

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
