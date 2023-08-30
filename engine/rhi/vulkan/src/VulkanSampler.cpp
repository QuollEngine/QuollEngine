#include "liquid/core/Base.h"

#include "VulkanHeaders.h"
#include "VulkanDeviceObject.h"
#include "VulkanSampler.h"
#include "VulkanError.h"
#include "VulkanMapping.h"

namespace quoll::rhi {

VulkanSampler::VulkanSampler(const SamplerDescription &description,
                             VulkanDeviceObject &device)
    : mDescription(description), mDevice(device) {
  VkSamplerCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;
  createInfo.minFilter = VulkanMapping::getFilter(description.minFilter);
  createInfo.magFilter = VulkanMapping::getFilter(description.magFilter);
  createInfo.addressModeU =
      VulkanMapping::getAddressMode(description.wrapModeU);
  createInfo.addressModeV =
      VulkanMapping::getAddressMode(description.wrapModeV);
  createInfo.addressModeW =
      VulkanMapping::getAddressMode(description.wrapModeW);
  createInfo.minLod = description.minLod;
  createInfo.maxLod =
      description.maxLod == 0.0f ? VK_LOD_CLAMP_NONE : description.maxLod;

  checkForVulkanError(vkCreateSampler(mDevice, &createInfo, nullptr, &mSampler),
                      "Failed to create sampler", description.debugName);

  mDevice.setObjectName(description.debugName + " sampler",
                        VK_OBJECT_TYPE_SAMPLER, static_cast<void *>(mSampler));
}

VulkanSampler::~VulkanSampler() {
  vkDestroySampler(mDevice, mSampler, nullptr);
}

} // namespace quoll::rhi
