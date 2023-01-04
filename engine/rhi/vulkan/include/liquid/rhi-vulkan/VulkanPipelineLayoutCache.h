#pragma once

#include "VulkanDeviceObject.h"
#include "VulkanShader.h"

namespace liquid::rhi {

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
   * @param info Descriptor set layout create info
   * @return Vulkan descriptor set layout
   */
  VkDescriptorSetLayout getOrCreateDescriptorLayout(
      const VulkanShader::ReflectionDescriptorSetLayout &info);

  /**
   * @brief Clear pipeline layout cache
   */
  void clear();

  /**
   * @brief Get global textures descriptor set layout
   *
   * Bindless textures
   *
   * @return Global textures descriptor set layout
   **/
  inline VkDescriptorSetLayout getGlobalTexturesDescriptorSetLayout() {
    return mDescriptorSetLayouts.at(0);
  }

private:
  /**
   * @brief Create desctiptor set layout
   *
   * @param info Descriptor set layout create info
   * @param flags Descriptor binding flags
   * @return Vulkan descriptor set layout
   */
  VkDescriptorSetLayout createDescriptorLayout(
      const VulkanShader::ReflectionDescriptorSetLayout &info,
      VkDescriptorBindingFlags flags);

  /**
   * @brief Create global textures descriptor layout
   *
   * Bindless textures
   */
  void createGlobalTexturesDescriptorLayout();

  /**
   * @brief Destroy all decriptor layouts
   */
  void destroyAllDescriptorLayouts();

private:
  VulkanDeviceObject &mDevice;

  std::vector<VulkanShader::ReflectionDescriptorSetLayout>
      mDescriptorSetLayoutData;

  std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
};

} // namespace liquid::rhi
