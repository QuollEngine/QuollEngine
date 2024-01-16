#pragma once

#include "quoll/rhi/RenderPassDescription.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

class VulkanRenderPass {
public:
  VulkanRenderPass(const RenderPassDescription &description,
                   VulkanDeviceObject &device,
                   const VulkanResourceRegistry &registry);

  ~VulkanRenderPass();

  VulkanRenderPass(const VulkanRenderPass &) = delete;
  VulkanRenderPass &operator=(const VulkanRenderPass &) = delete;
  VulkanRenderPass(VulkanRenderPass &&) = delete;
  VulkanRenderPass &operator=(VulkanRenderPass &&) = delete;

  inline VkRenderPass getRenderPass() const { return mRenderPass; }

  inline const std::vector<VkClearValue> &getClearValues() const {
    return mClearValues;
  }

private:
  VkAttachmentDescription getVulkanAttachmentDescription(
      const RenderPassAttachmentDescription &description,
      const VulkanResourceRegistry &registry);

private:
  VulkanDeviceObject &mDevice;
  VkRenderPass mRenderPass = VK_NULL_HANDLE;
  std::vector<VkClearValue> mClearValues;
};

} // namespace quoll::rhi
