#pragma once

#include "liquid/rhi/DescriptorLayoutDescription.h"
#include "liquid/rhi/RenderHandle.h"

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
  /**
   * @brief Create Vulkan pipeline layout cache
   *
   * @param device Vulkan device
   */
  VulkanPipelineLayoutCache(VulkanDeviceObject &device);

  VulkanPipelineLayoutCache(const VulkanPipelineLayoutCache &) = delete;
  VulkanPipelineLayoutCache(VulkanPipelineLayoutCache &&) = delete;
  VulkanPipelineLayoutCache &
  operator=(const VulkanPipelineLayoutCache &) = delete;
  VulkanPipelineLayoutCache &operator=(VulkanPipelineLayoutCache &&) = delete;

  /**
   * @brief Destroy pipeline layout cache
   */
  ~VulkanPipelineLayoutCache();

  /**
   * @brief Get or create descriptor layout
   *
   * @param description Descriptor layout description
   * @return Descriptor layout
   */
  DescriptorLayoutHandle
  getOrCreateDescriptorLayout(const DescriptorLayoutDescription &description);

  /**
   * @brief Clear pipeline layout cache
   */
  void clear();

  /**
   * @brief Get Vulkan descriptor set layout
   *
   * @param handle Descriptor layout handle
   * @return Vulkan descriptor set layout
   */
  inline VkDescriptorSetLayout
  getVulkanDescriptorSetLayout(DescriptorLayoutHandle handle) {
    return mDescriptorSetLayouts.at(static_cast<size_t>(handle) - 1);
  }

  /**
   * @brief Get descriptor layout description
   *
   * @param handle Descriptor layout handle
   * @return Descriptor l;ayout description
   */
  inline const DescriptorLayoutDescription &
  getDescriptorLayoutDescription(DescriptorLayoutHandle handle) {
    return mDescriptorLayoutDescriptions.at(static_cast<size_t>(handle) - 1);
  }

private:
  /**
   * @brief Create descriptor layout
   *
   * @param info Descriptor layout description
   * @return Vulkan descriptor set layout
   */
  VkDescriptorSetLayout
  createDescriptorLayout(const DescriptorLayoutDescription &description);

  /**
   * @brief Destroy all descriptor layouts
   */
  void destroyAllDescriptorLayouts();

private:
  VulkanDeviceObject &mDevice;

  std::vector<DescriptorLayoutDescription> mDescriptorLayoutDescriptions;
  std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
};

} // namespace quoll::rhi
