#pragma once

#include "quoll/rhi/FramebufferDescription.h"
#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"

namespace quoll::rhi {

class VulkanFramebuffer : NoCopyMove {
public:
  VulkanFramebuffer(const FramebufferDescription &description,
                    VulkanDeviceObject &device,
                    const VulkanResourceRegistry &registry);

  ~VulkanFramebuffer();

  inline VkFramebuffer getFramebuffer() const { return mFramebuffer; }

private:
  VulkanDeviceObject &mDevice;
  VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
