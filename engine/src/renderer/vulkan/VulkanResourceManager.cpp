#include "core/Base.h"
#include "VulkanResourceManager.h"
#include "VulkanMaterialResourceBinder.h"

namespace liquid {

VulkanResourceManager::VulkanResourceManager(
    VulkanDescriptorManager *descriptorManager_,
    VulkanPipelineBuilder *pipelineBuilder_, VkRenderPass renderPass_,
    uint32_t subpass_)
    : descriptorManager(descriptorManager_), pipelineBuilder(pipelineBuilder_),
      renderPass(renderPass_), subpass(subpass_) {}

SharedPtr<MaterialResourceBinder>
VulkanResourceManager::createMaterialResourceBinder(Material *material) {
  return std::make_shared<VulkanMaterialResourceBinder>(
      material, descriptorManager, pipelineBuilder, renderPass, subpass);
}

} // namespace liquid
