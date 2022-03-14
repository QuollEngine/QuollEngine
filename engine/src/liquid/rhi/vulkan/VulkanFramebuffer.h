#pragma once

#include "liquid/rhi/FramebufferDescription.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"

namespace liquid::rhi {

class VulkanFramebuffer {
public:
  /**
   * @brief Create Vulkan framebuffer
   *
   * @param description Framebuffer description
   * @param device Vulkan device
   * @param registry Resource registry
   */
  VulkanFramebuffer(const FramebufferDescription &description,
                    VulkanDeviceObject &device,
                    const VulkanResourceRegistry &registry);

  /**
   * @brief Destroy framebuffer
   */
  ~VulkanFramebuffer();

  VulkanFramebuffer(const VulkanFramebuffer &) = delete;
  VulkanFramebuffer &operator=(const VulkanFramebuffer &) = delete;
  VulkanFramebuffer(VulkanFramebuffer &&) = delete;
  VulkanFramebuffer &operator=(VulkanFramebuffer &&) = delete;

  /**
   * @brief Get vulkan framebuffer
   *
   * @return Vulkan framebuffer
   */
  inline VkFramebuffer getFramebuffer() const { return mFramebuffer; }

private:
  VulkanDeviceObject &mDevice;
  VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
};

} // namespace liquid::rhi
