#pragma once

#include "quoll/rhi/SamplerDescription.h"

namespace quoll::rhi {

class VulkanSampler {
public:
  VulkanSampler(const SamplerDescription &description,
                VulkanDeviceObject &device);

  ~VulkanSampler();

  VulkanSampler(const VulkanSampler &) = delete;
  VulkanSampler &operator=(const VulkanSampler &) = delete;
  VulkanSampler(VulkanSampler &&) = delete;
  VulkanSampler &operator=(VulkanSampler &&) = delete;

  inline VkSampler getSampler() const { return mSampler; }

private:
  SamplerDescription mDescription;

  VulkanDeviceObject &mDevice;

  VkSampler mSampler = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
