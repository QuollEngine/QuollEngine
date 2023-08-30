#pragma once

#include "quoll/rhi/RenderPassDescription.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

/**
 * @brief Vulkan render pass
 */
class VulkanRenderPass {
public:
  /**
   * @brief Create render pass
   *
   * @param description Render pass description
   * @param device Vulkan device
   * @param registry Resource registry
   */
  VulkanRenderPass(const RenderPassDescription &description,
                   VulkanDeviceObject &device,
                   const VulkanResourceRegistry &registry);

  /**
   * @brief Destroy render pass
   */
  ~VulkanRenderPass();

  VulkanRenderPass(const VulkanRenderPass &) = delete;
  VulkanRenderPass &operator=(const VulkanRenderPass &) = delete;
  VulkanRenderPass(VulkanRenderPass &&) = delete;
  VulkanRenderPass &operator=(VulkanRenderPass &&) = delete;

  /**
   * @brief Get Vulkan render pass
   *
   * @return Vulkan render pass
   */
  inline VkRenderPass getRenderPass() const { return mRenderPass; }

  /**
   * @brief Get clear values
   *
   * @return Clear values
   */
  inline const std::vector<VkClearValue> &getClearValues() const {
    return mClearValues;
  }

private:
  /**
   * @brief Get Vulkan attachment description
   *
   * @param description Render pass attachment description
   * @param registry Vulkan resource registry
   * @return Vulkan attachment description
   */
  VkAttachmentDescription getVulkanAttachmentDescription(
      const RenderPassAttachmentDescription &description,
      const VulkanResourceRegistry &registry);

private:
  VulkanDeviceObject &mDevice;
  VkRenderPass mRenderPass = VK_NULL_HANDLE;
  std::vector<VkClearValue> mClearValues;
};

} // namespace quoll::rhi
