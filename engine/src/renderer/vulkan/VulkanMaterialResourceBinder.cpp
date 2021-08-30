#include "core/Base.h"
#include "VulkanMaterialResourceBinder.h"
#include "VulkanShader.h"
#include "VulkanHardwareBuffer.h"

namespace liquid {

VulkanMaterialResourceBinder::VulkanMaterialResourceBinder(
    Material *material, VulkanDescriptorManager *descriptorManager_,
    VulkanPipelineBuilder *pipelineBuilder, VkRenderPass renderPass,
    uint32_t subpass)
    : descriptorManager(descriptorManager_) {

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module =
      std::dynamic_pointer_cast<VulkanShader>(material->getVertexShader())
          ->getShaderModule();
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module =
      std::dynamic_pointer_cast<VulkanShader>(material->getFragmentShader())
          ->getShaderModule();
  fragShaderStageInfo.pName = "main";

  std::array<VkPipelineShaderStageCreateInfo, 2> stages{vertShaderStageInfo,
                                                        fragShaderStageInfo};

  graphicsPipeline =
      pipelineBuilder->build(stages.data(), VK_POLYGON_MODE_FILL, renderPass,
                             subpass, material->getCullMode());
  wireframeGraphicsPipeline = pipelineBuilder->build(
      stages.data(), VK_POLYGON_MODE_LINE, renderPass, subpass, CullMode::None);

  materialDescriptorSet = descriptorManager->createMaterialDescriptorSet(
      std::dynamic_pointer_cast<VulkanHardwareBuffer>(
          material->getUniformBuffer()),
      material->getTextures());
}

VulkanMaterialResourceBinder::~VulkanMaterialResourceBinder() {
  if (materialDescriptorSet) {
    vkFreeDescriptorSets(descriptorManager->getDevice(),
                         descriptorManager->getDescriptorPool(), 1,
                         &materialDescriptorSet);
  }
}

} // namespace liquid
