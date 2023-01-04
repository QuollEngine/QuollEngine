#pragma once

#include "liquid/rhi/Descriptor.h"

#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"
#include "VulkanDescriptorPool.h"

#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

/**
 * @brief Vulkan descriptor manager
 *
 * Automatically creates and retrieves
 * descriptors based on hash
 */
class VulkanDescriptorManager {
public:
  /**
   * @brief Create Vulkan descriptor manager
   *
   * @param device Vulkan device
   * @param registry Vulkan resource registry
   * @param descriptorPool Descriptor pool
   */
  VulkanDescriptorManager(VulkanDeviceObject &device,
                          const VulkanResourceRegistry &registry,
                          VulkanDescriptorPool &descriptorPool);

  VulkanDescriptorManager(const VulkanDescriptorManager &) = delete;
  VulkanDescriptorManager(VulkanDescriptorManager &&) = delete;
  VulkanDescriptorManager &operator=(const VulkanDescriptorManager &) = delete;
  VulkanDescriptorManager &operator=(VulkanDescriptorManager &&) = delete;

  /**
   * @brief Get Vulkan descriptor set
   *
   * Gets descriptor set from cache or creates
   * descriptors and returns it
   *
   * @param descriptor Descriptor
   * @param layout Vulkan descriptor layout
   * @return Vulkan descriptor set
   */
  VkDescriptorSet getOrCreateDescriptor(const Descriptor &descriptor,
                                        VkDescriptorSetLayout layout);

  /**
   * @brief Clear cache
   */
  void clear();

  /**
   * @brief Create global textures descriptor set
   *
   * Bindless textures
   *
   * @param layout Global texture descriptor set layout
   */
  void createGlobalTexturesDescriptorSet(DescriptorLayoutHandle layout);

  /**
   * @brief Add global texture to global textures descriptor
   *
   * @param handle Texture handle
   */
  void addGlobalTexture(rhi::TextureHandle handle);

private:
  /**
   * @brief Create descriptor set
   *
   * @param descriptor Descriptor
   * @param layout Descriptor layout
   * @return Vulkan descriptor set
   */
  VkDescriptorSet createDescriptorSet(const Descriptor &descriptor,
                                      VkDescriptorSetLayout layout);

  /**
   * @brief Create hash from descriptor and layout
   *
   * @param descriptor Descriptor
   * @param layout Descriptor layout
   * @return Hash code
   */
  String createHash(const Descriptor &descriptor, VkDescriptorSetLayout layout);

private:
  std::unordered_map<String, VkDescriptorSet> mDescriptorCache;
  VulkanDescriptorPool &mDescriptorPool;
  VkDevice mDevice;

  const rhi::VulkanResourceRegistry &mRegistry;

  n::Descriptor mGlobalTexturesDescriptor;
};

} // namespace liquid::rhi
