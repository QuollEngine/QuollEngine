#pragma once

#include "liquid/rhi/Descriptor.h"

#include "VulkanResourceRegistry.h"
#include "VulkanDeviceObject.h"

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
   */
  VulkanDescriptorManager(VulkanDeviceObject &device,
                          const VulkanResourceRegistry &registry);

  VulkanDescriptorManager(const VulkanDescriptorManager &) = delete;
  VulkanDescriptorManager(VulkanDescriptorManager &&) = delete;
  VulkanDescriptorManager &operator=(const VulkanDescriptorManager &) = delete;
  VulkanDescriptorManager &operator=(VulkanDescriptorManager &&) = delete;

  /**
   * @brief Destroy descriptor manager
   */
  ~VulkanDescriptorManager();

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
   * @brief Get descriptor cache size
   *
   * @return Descriptor cache size
   */
  inline size_t getCacheSize() { return mDescriptorCache.size(); }

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
  void createGlobalTexturesDescriptorSet(VkDescriptorSetLayout layout);

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
   * @brief Allocate descriptor set
   *
   * @param layout Vulkan descriptor layout
   * @return Vulkan descriptor set
   */
  VkDescriptorSet allocateDescriptorSet(VkDescriptorSetLayout layout);

  /**
   * @brief Create descriptor pool
   */
  void createDescriptorPool();

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
  VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
  VkDevice mDevice;

  const rhi::VulkanResourceRegistry &mRegistry;

  VkDescriptorSet mGlobalTexturesDescriptorSet = VK_NULL_HANDLE;
};

} // namespace liquid::rhi
