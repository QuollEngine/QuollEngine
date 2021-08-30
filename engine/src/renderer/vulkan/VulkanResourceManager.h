#pragma once

#include "renderer/ResourceManager.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanDescriptorManager.h"

namespace liquid {

class VulkanResourceManager : public ResourceManager {
public:
  /**
   * @brief Create Vulkan resource manager
   *
   * @param descriptorManager Descriptor manager
   * @param pipelineBuilder Pipeline builder
   * @param renderPass Vulkan render pass
   * @param subpass Vulkan render pass subpass index
   */
  VulkanResourceManager(VulkanDescriptorManager *descriptorManager,
                        VulkanPipelineBuilder *pipelineBuilder,
                        VkRenderPass renderPass, uint32_t subpass);

  /**
   * @brief Create Vulkan material resource binder
   *
   * @param material Material
   * @return Vulkan material resource binder
   */
  SharedPtr<MaterialResourceBinder>
  createMaterialResourceBinder(Material *material) override;

private:
  VulkanDescriptorManager *descriptorManager;
  VulkanPipelineBuilder *pipelineBuilder;
  VkRenderPass renderPass;
  uint32_t subpass;
};

} // namespace liquid
