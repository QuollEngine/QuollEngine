#pragma once

#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"

namespace liquid::experimental {

class VulkanUploadContext {
  using SubmitFn = std::function<void(VkCommandBuffer)>;

public:
  /**
   * @brief Create upload context
   *
   * @param device Vulkan device
   * @param pool Command pool
   * @param queue Vulkan queue
   */
  VulkanUploadContext(VulkanDeviceObject &device, VulkanCommandPool &pool,
                      VulkanQueue &queue);

  /**
   * @brief Destroy upload context
   */
  ~VulkanUploadContext();

  VulkanUploadContext(const VulkanUploadContext &) = delete;
  VulkanUploadContext(VulkanUploadContext &&) = delete;
  VulkanUploadContext &operator=(const VulkanUploadContext &) = delete;
  VulkanUploadContext &operator=(VulkanUploadContext &&) = delete;

  /**
   * @brief Submit for upload
   *
   * @param submitFn Submit function
   */
  void submit(const SubmitFn &submitFn) const;

private:
  /**
   * @brief Create upload fence
   */
  void createFence();

private:
  VkFence mUploadFence = VK_NULL_HANDLE;
  RenderCommandList mCommandList;
  VulkanQueue &mQueue;
  VulkanDeviceObject &mDevice;
};

} // namespace liquid::experimental
