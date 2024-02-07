#pragma once

#include "quoll/rhi/SamplerDescription.h"

namespace quoll::rhi {

class VulkanSampler : NoCopyMove {
public:
  VulkanSampler(const SamplerDescription &description,
                VulkanDeviceObject &device);

  ~VulkanSampler();

  inline VkSampler getSampler() const { return mSampler; }

private:
  SamplerDescription mDescription;

  VulkanDeviceObject &mDevice;

  VkSampler mSampler = VK_NULL_HANDLE;
};

} // namespace quoll::rhi
