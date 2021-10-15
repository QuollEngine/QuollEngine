#pragma once

#include "core/Base.h"
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
  void create(const VulkanContext &context);

  /**
   * @brief Destroy upload context
   */
  void destroy();

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
