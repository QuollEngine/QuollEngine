#pragma once

#include "quoll/rhi/RenderPassDescription.h"
#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"

namespace quoll::rhi {

class VulkanRenderPass : NoCopyMove {
public:
  VulkanRenderPass(const RenderPassDescription &description,
                   VulkanDeviceObject &device,
                   const VulkanResourceRegistry &registry);

  ~VulkanRenderPass();

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
