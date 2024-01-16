#pragma once

#include "quoll/rhi/Descriptor.h"

#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"
#include "VulkanPipelineLayoutCache.h"

namespace quoll::rhi {

class VulkanDescriptorPool {
public:
  VulkanDescriptorPool(VulkanDeviceObject &device,
                       VulkanResourceRegistry &registry,
                       VulkanPipelineLayoutCache &pipelineLayoutCache);

  ~VulkanDescriptorPool();

  Descriptor createDescriptor(DescriptorLayoutHandle layout);

  void reset();

  inline VkDescriptorSet getDescriptorSet(DescriptorHandle handle) const {
    return mDescriptorSets.at(static_cast<usize>(handle) - 1);
  }

  inline usize getDescriptorsCount() const { return mDescriptorSets.size(); }

  inline VkDescriptorSetLayout
  getLayoutFromDescriptor(VkDescriptorSet descriptor) const {
    return mDescriptorLayoutMap.at(descriptor);
  }

private:
  void createDescriptorPool();

private:
  VulkanDeviceObject &mDevice;
  VulkanResourceRegistry &mRegistry;
  VulkanPipelineLayoutCache &mPipelineLayoutCache;

  VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> mDescriptorSets;

  std::unordered_map<VkDescriptorSet, VkDescriptorSetLayout>
      mDescriptorLayoutMap;
};

} // namespace quoll::rhi
