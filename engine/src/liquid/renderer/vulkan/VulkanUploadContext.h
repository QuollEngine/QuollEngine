#pragma once

#include "../../rhi/vulkan/VulkanRenderDevice.h"

namespace liquid {

class VulkanUploadContext {
  using SubmitFn = std::function<void(VkCommandBuffer)>;

public:
  /**
   * @brief Create upload context
   *
   * @param device Vulkan device
   */
  VulkanUploadContext(experimental::VulkanRenderDevice *device);

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
   * @param graphicsFamily Graphics family index
   */
  void createCommandPool(uint32_t graphicsFamily);

private:
  VkFence uploadFence = VK_NULL_HANDLE;
  VkCommandPool uploadCommandPool = VK_NULL_HANDLE;
  VkDevice device = nullptr;
  VkQueue graphicsQueue = nullptr;
};

} // namespace liquid
