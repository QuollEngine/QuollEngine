#include "core/Base.h"
#include "scene/Vertex.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanVertexInputDescription.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanError.h"
#include "core/EngineGlobals.h"

namespace liquid {

VulkanPipelineBuilder::VulkanPipelineBuilder(
    VkDevice device_, VulkanDescriptorManager *descriptorManager)
    : device(device_) {
  createPipelineLayout(descriptorManager);
}

VulkanPipelineBuilder::~VulkanPipelineBuilder() {
  if (pipelineLayout) {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    LOG_DEBUG("[Vulkan] Pipeline layout destroyed");
  }
}

VulkanPipeline
VulkanPipelineBuilder::build(VkPipelineShaderStageCreateInfo *shaderStages,
                             VkPolygonMode polygonMode, VkRenderPass renderPass,
                             uint32_t subpass, const CullMode &cullMode) const {
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = polygonMode;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = getVulkanCullMode(cullMode);
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  // multisampling
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional

  // Color blending
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional

  // Dynamic state
  std::array<VkDynamicState, 2> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT,
                                              VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.flags = 0;
  dynamicState.pNext = nullptr;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // Viewport state
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = nullptr;
  viewportState.scissorCount = 1;
  viewportState.pScissors = nullptr;

  // depth stencil
  VkPipelineDepthStencilStateCreateInfo depthStencilState{};
  depthStencilState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilState.pNext = nullptr;
  depthStencilState.depthTestEnable = VK_TRUE;
  depthStencilState.depthWriteEnable = VK_TRUE;
  depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencilState.depthBoundsTestEnable = VK_FALSE;
  depthStencilState.minDepthBounds = 0.0f;
  depthStencilState.maxDepthBounds = 1.0f;
  depthStencilState.stencilTestEnable = VK_FALSE;

  // vertex input
  VkPipelineVertexInputStateCreateInfo vertexInput{};
  auto description = getVulkanVertexInputDescription<Vertex>();

  vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInput.vertexBindingDescriptionCount =
      static_cast<uint32_t>(description.bindings.size());
  vertexInput.pVertexBindingDescriptions = description.bindings.data();
  vertexInput.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(description.attributes.size());
  vertexInput.pVertexAttributeDescriptions = description.attributes.data();

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInput;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencilState;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = subpass;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  VkPipeline pipeline = VK_NULL_HANDLE;
  checkForVulkanError(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                                &pipelineInfo, nullptr,
                                                &pipeline),
                      "Failed to create graphics pipeline");

  return VulkanPipeline(device, pipeline);
}

void VulkanPipelineBuilder::createPipelineLayout(
    VulkanDescriptorManager *descriptorManager) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(VulkanStandardPushConstants);
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  auto descriptorSetLayouts = descriptorManager->getDescriptorLayouts();
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  checkForVulkanError(vkCreatePipelineLayout(device, &pipelineLayoutInfo,
                                             nullptr, &pipelineLayout),
                      "Failed to create pipeline layout");
  LOG_DEBUG("[Vulkan] Pipeline layout created");
}

VkCullModeFlags
VulkanPipelineBuilder::getVulkanCullMode(const CullMode &cullMode) {
  switch (cullMode) {
  case CullMode::Back:
    return VK_CULL_MODE_BACK_BIT;
  case CullMode::Front:
    return VK_CULL_MODE_FRONT_BIT;
  case CullMode::FrontAndBack:
    return VK_CULL_MODE_FRONT_AND_BACK;
  case CullMode::None:
  default:
    return VK_CULL_MODE_NONE;
  }
}

} // namespace liquid
