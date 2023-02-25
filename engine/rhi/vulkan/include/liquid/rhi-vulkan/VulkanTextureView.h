#pragma once

#include "liquid/rhi/TextureViewDescription.h"

#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"

namespace liquid::rhi {

/**
 * @brief Vulkan texture view
 */
class VulkanTextureView {
public:
  /**
   * @brief Create texture view
   *
   * @param description Texture view description
   * @param registry Vulkan resource registry
   * @param device
   */
  VulkanTextureView(const TextureViewDescription &description,
                    VulkanResourceRegistry &registry,
                    VulkanDeviceObject &device);

  /**
   * @brief Destroy texture view
   */
  ~VulkanTextureView();

  VulkanTextureView(const VulkanTextureView &) = delete;
  VulkanTextureView &operator=(const VulkanTextureView &) = delete;
  VulkanTextureView(VulkanTextureView &&) = delete;
  VulkanTextureView &operator=(VulkanTextureView &&) = delete;

  /**
   * @brief Get Vulkan image view handle
   *
   * @return Vulkan image view handle
   */
  inline VkImageView getImageView() { return mImageView; }

  /**
   * @brief Get Vulkan sampler handle
   *
   * @return Vulkan sampler handle
   */
  inline VkSampler getSampler() { return mSampler; }

private:
  VulkanDeviceObject &mDevice;
  VkImageView mImageView = VK_NULL_HANDLE;
  VkSampler mSampler = VK_NULL_HANDLE;
};

} // namespace liquid::rhi
