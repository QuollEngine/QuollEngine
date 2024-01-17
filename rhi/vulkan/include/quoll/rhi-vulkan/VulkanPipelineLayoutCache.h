#pragma once

#include "quoll/rhi/DescriptorLayoutDescription.h"
#include "quoll/rhi/RenderHandle.h"
#include "VulkanDeviceObject.h"
#include "VulkanShader.h"

namespace quoll::rhi {

/**
 * @brief Vulkan pipeline layout cache
 *
 * Stores references to descriptor and
 * pipeline layouts and reuses descriptor
 * or pipeline layouts if the provided
 * create information matches what's
 * already in the cache
 */
class VulkanPipelineLayoutCache {
public:
  VulkanPipelineLayoutCache(VulkanDeviceObject &device);

  VulkanPipelineLayoutCache(const VulkanPipelineLayoutCache &) = delete;
  VulkanPipelineLayoutCache(VulkanPipelineLayoutCache &&) = delete;
  VulkanPipelineLayoutCache &
  operator=(const VulkanPipelineLayoutCache &) = delete;
  VulkanPipelineLayoutCache &operator=(VulkanPipelineLayoutCache &&) = delete;

  ~VulkanPipelineLayoutCache();

  DescriptorLayoutHandle
  getOrCreateDescriptorLayout(const DescriptorLayoutDescription &description);

  void clear();

  inline VkDescriptorSetLayout
  getVulkanDescriptorSetLayout(DescriptorLayoutHandle handle) {
    return mDescriptorSetLayouts.at(static_cast<usize>(handle) - 1);
  }

  inline const DescriptorLayoutDescription &
  getDescriptorLayoutDescription(DescriptorLayoutHandle handle) {
    return mDescriptorLayoutDescriptions.at(static_cast<usize>(handle) - 1);
  }

private:
  VkDescriptorSetLayout
  createDescriptorLayout(const DescriptorLayoutDescription &description);

  void destroyAllDescriptorLayouts();

private:
  VulkanDeviceObject &mDevice;

  std::vector<DescriptorLayoutDescription> mDescriptorLayoutDescriptions;
  std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
};

} // namespace quoll::rhi
