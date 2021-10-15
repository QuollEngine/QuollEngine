#pragma once

#include "renderer/MaterialResourceBinder.h"
#include "renderer/Material.h"

#include "VulkanDescriptorManager.h"
#include "VulkanPipelineBuilder.h"

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
   * @param pipelineBuilder Pipeline builder
   * @param renderPass Vulkan render pass
   * @param subpass Vulkan render pass subpass index
   */
  VulkanMaterialResourceBinder(Material *material,
                               VulkanDescriptorManager *descriptorManager,
                               VulkanPipelineBuilder *pipelineBuilder,
                               VkRenderPass renderPass, uint32_t subpass);

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
   * @brief Get graphics pipeline
   *
   * Returns wireframe pipeline if wireframeMode is true
   *
   * @param wireframeMode Wireframe mode state
   * @return Graphics pipeline
   */
  inline VkPipeline getGraphicsPipeline(bool wireframeMode) const {
    return wireframeMode ? wireframeGraphicsPipeline.getPipeline()
                         : graphicsPipeline.getPipeline();
  }

  /**
   * @brief Get material descriptor set
   *
   * @return Material descriptor set
   */
  inline const VkDescriptorSet getDescriptorSet() const {
    return materialDescriptorSet;
  }

private:
  VulkanPipeline graphicsPipeline;
  VulkanPipeline wireframeGraphicsPipeline;

  VkDescriptorSet materialDescriptorSet = VK_NULL_HANDLE;
  VulkanDescriptorManager *descriptorManager = VK_NULL_HANDLE;
};

} // namespace liquid
