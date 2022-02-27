#pragma once

#include "VulkanContext.h"

namespace liquid {

class VulkanUploadContext {
  using SubmitFn = std::function<void(VkCommandBuffer)>;

public:
  /**
   * @brief Create upload context
   *
   * @param context Vulkan context
   */
  VulkanUploadContext(const VulkanContext &context);

  /**
   * @brief Destroy upload context
   */
  ~VulkanUploadContext();

  /**
   * @brief Default constructor
   */
  VulkanUploadContext() = default;

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

  /**
   * @brief Create command pool for uploads
   *
   * @param context Vulkan context
   */
  void createCommandPool(const VulkanContext &context);

private:
  VkFence uploadFence = VK_NULL_HANDLE;
  VkCommandPool uploadCommandPool = VK_NULL_HANDLE;
  VkDevice device = nullptr;
  VkQueue graphicsQueue = nullptr;
};

} // namespace liquid
