#pragma once

#include "renderer/MaterialResourceBinder.h"
#include "renderer/Material.h"

#include "VulkanDescriptorManager.h"

namespace liquid {

class VulkanMaterialResourceBinder : public MaterialResourceBinder {
public:
  /**
   * @brief Create Vulkan material resource binder
   *
   * Creates pipelines and descriptor sets
   *
   * @param material Material
   * @param descriptorManager Descriptor manager
   */
  VulkanMaterialResourceBinder(Material *material,
                               VulkanDescriptorManager *descriptorManager);

  /**
   * @brief Destroy Vulkan material resource binder
   *
   * Destroys pipelines and descriptor set
   */
  ~VulkanMaterialResourceBinder();

  VulkanMaterialResourceBinder(const VulkanMaterialResourceBinder &rhs) =
      delete;
  VulkanMaterialResourceBinder(VulkanMaterialResourceBinder &&rhs) = delete;
  VulkanMaterialResourceBinder &
  operator=(const VulkanMaterialResourceBinder &rhs) = delete;
  VulkanMaterialResourceBinder &
  operator=(VulkanMaterialResourceBinder &&rhs) = delete;

  /**
   * @brief Get material descriptor set
   *
   * @return Material descriptor set
   */
  inline const VkDescriptorSet getDescriptorSet() const {
    return materialDescriptorSet;
  }

private:
  VkDescriptorSet materialDescriptorSet = VK_NULL_HANDLE;
  VulkanDescriptorManager *descriptorManager = nullptr;
};

} // namespace liquid
