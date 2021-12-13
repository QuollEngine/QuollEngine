#pragma once

#include "renderer/MaterialResourceBinder.h"
#include "renderer/Material.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorManager.h"

namespace liquid {

class VulkanDeferredMaterialBinder : public MaterialResourceBinder {
public:
  /**
   * @brief Create Vulkan material resource binder
   *
   * Creates pipelines and descriptor sets
   *
   * @param material Material
   * @param descriptorManager Descriptor manager
   */
  VulkanDeferredMaterialBinder(Material *material,
                               VulkanDescriptorManager *descriptorManager);

  /**
   * @brief Destroy Vulkan material resource binder
   *
   * Destroys descriptor set
   */
  ~VulkanDeferredMaterialBinder();

  VulkanDeferredMaterialBinder(const VulkanDeferredMaterialBinder &rhs) =
      delete;
  VulkanDeferredMaterialBinder(VulkanDeferredMaterialBinder &&rhs) = delete;
  VulkanDeferredMaterialBinder &
  operator=(const VulkanDeferredMaterialBinder &rhs) = delete;
  VulkanDeferredMaterialBinder &
  operator=(VulkanDeferredMaterialBinder &&rhs) = delete;

  /**
   * @brief Get material descriptor set
   *
   * @param layout Descriptor set layout
   * @return Material descriptor set
   */
  VkDescriptorSet getDescriptorSet(VkDescriptorSetLayout layout);

private:
  VkDescriptorSet materialDescriptorSet = VK_NULL_HANDLE;
  VulkanDescriptorManager *descriptorManager = nullptr;
  Material *material;
};

} // namespace liquid
