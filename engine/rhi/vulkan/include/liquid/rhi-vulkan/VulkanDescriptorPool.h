#pragma once

#include "liquid/rhi/Descriptor.h"

#include "VulkanDeviceObject.h"
#include "VulkanResourceRegistry.h"
#include "VulkanPipelineLayoutCache.h"

namespace quoll::rhi {

/**
 * @brief Vulkan descriptor pool
 */
class VulkanDescriptorPool {
public:
  /**
   * @brief Create descriptor pool
   *
   * @param device Vulkan device
   * @param registry Resource registry
   * @param pipelineLayoutCache Pipeline layout cache
   */
  VulkanDescriptorPool(VulkanDeviceObject &device,
                       VulkanResourceRegistry &registry,
                       VulkanPipelineLayoutCache &pipelineLayoutCache);

  /**
   * @brief Destroy Vulkan descriptor pool
   */
  ~VulkanDescriptorPool();

  /**
   * @brief Create descriptor
   *
   * @param layout Descriptor layout
   * @return Descriptor
   */
  Descriptor createDescriptor(DescriptorLayoutHandle layout);

  /**
   * @brief Reset descriptor pool
   */
  void reset();

  /**
   * @brief Get Vulkan descriptor set
   *
   * @param handle Descriptor handle
   * @return Vulkan descriptor set
   */
  inline VkDescriptorSet getDescriptorSet(DescriptorHandle handle) const {
    return mDescriptorSets.at(static_cast<size_t>(handle) - 1);
  }

  /**
   * @brief Get number of descriptors
   *
   * @return Number of descriptors
   */
  inline size_t getDescriptorsCount() const { return mDescriptorSets.size(); }

  /**
   * @brief Get layout from descriptor
   *
   * @param descriptor Vulkan descriptor
   * @return Vulkan descriptor set layout
   */
  inline VkDescriptorSetLayout
  getLayoutFromDescriptor(VkDescriptorSet descriptor) const {
    return mDescriptorLayoutMap.at(descriptor);
  }

private:
  /**
   * @brief Create Vulkan descriptor pool
   */
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
