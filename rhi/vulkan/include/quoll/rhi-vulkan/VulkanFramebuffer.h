#pragma once

#include "quoll/rhi/FramebufferDescription.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

class VulkanFramebuffer {
public:
  VulkanFramebuffer(const FramebufferDescription &description,
                    VulkanDeviceObject &device,
                    const VulkanResourceRegistry &registry);

  ~VulkanFramebuffer();

  VulkanFramebuffer(const VulkanFramebuffer &) = delete;
  VulkanFramebuffer &operator=(const VulkanFramebuffer &) = delete;
  VulkanFramebuffer(VulkanFramebuffer &&) = delete;
  VulkanFramebuffer &operator=(VulkanFramebuffer &&) = delete;

  inline VkFramebuffer getFramebuffer() const { return mFramebuffer; }

private:
  VulkanDeviceObject &mDevice;
  VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
