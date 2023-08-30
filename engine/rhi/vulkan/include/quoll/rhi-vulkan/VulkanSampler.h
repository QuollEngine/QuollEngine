#pragma once

#include "quoll/rhi/SamplerDescription.h"

namespace quoll::rhi {

/**
 * @brief Vulkan sampler
 *
 * Manages sampler lifecycle and state
 */
class VulkanSampler {
public:
  /**
   * @brief Create Vulkan sampler
   *
   * @param description Sampler description
   * @param device Vulkan device
   */
  VulkanSampler(const SamplerDescription &description,
                VulkanDeviceObject &device);

  /**
   * @brief Destroy Vulkan sampler
   */
  ~VulkanSampler();

  VulkanSampler(const VulkanSampler &) = delete;
  VulkanSampler &operator=(const VulkanSampler &) = delete;
  VulkanSampler(VulkanSampler &&) = delete;
  VulkanSampler &operator=(VulkanSampler &&) = delete;

  /**
   * @brief Get Vulkan sampler
   *
   * @return Vulkan sampler
   */
  inline VkSampler getSampler() const { return mSampler; }

private:
  SamplerDescription mDescription;

  VulkanDeviceObject &mDevice;

  VkSampler mSampler = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
