#pragma once

#include "liquid/rhi/RenderPassDescription.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"

namespace liquid::rhi {

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

private:
  VulkanDeviceObject &mDevice;
  VkRenderPass mRenderPass = VK_NULL_HANDLE;
};

} // namespace liquid::rhi
