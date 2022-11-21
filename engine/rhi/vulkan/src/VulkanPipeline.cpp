#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanRenderPass.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanMapping.h"
#include "VulkanError.h"
#include "VulkanLog.h"

namespace liquid::rhi {

VulkanPipeline::VulkanPipeline(const PipelineDescription &description,
                               VulkanDeviceObject &device,
                               const VulkanResourceRegistry &registry)
    : mDevice(device) {

  std::array<VulkanShader *, 2> shaders{
      registry.getShaders().at(description.vertexShader).get(),
      registry.getShaders().at(description.fragmentShader).get(),
  };

  std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
  for (size_t i = 0; i < 2; ++i) {
    stages.at(i).sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages.at(i).pName = "main";
    stages.at(i).module = shaders.at(i)->getShaderModule();
    stages.at(i).stage = shaders.at(i)->getShaderStage();
  }

  // Pipeline Layout
  std::map<uint32_t, VkDescriptorSetLayout> descriptorLayoutsMap;
  std::vector<VkDescriptorSetLayout> descriptorLayoutsRaw;
  std::vector<VkPushConstantRange> pushConstantRanges;

  for (auto &shader : shaders) {
    const auto &reflection = shader->getReflectionData();
    for (auto &[set, bindings] : reflection.descriptorSetLayouts) {

      std::vector<VkDescriptorBindingFlags> bindingFlags(
          bindings.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

      VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{};
      bindingFlagsCreateInfo.sType =
          VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
      bindingFlagsCreateInfo.pNext = nullptr;
      bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();
      bindingFlagsCreateInfo.bindingCount =
          static_cast<uint32_t>(bindingFlags.size());

      VkDescriptorSetLayout layout = VK_NULL_HANDLE;
      VkDescriptorSetLayoutCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      createInfo.flags = 0;
      createInfo.pNext = &bindingFlagsCreateInfo;
      createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
      createInfo.pBindings = bindings.data();
      checkForVulkanError(
          vkCreateDescriptorSetLayout(mDevice, &createInfo, nullptr, &layout),
          "Failed to create descriptor set layout");

      descriptorLayoutsMap.insert({set, layout});
    }

    for (auto &x : reflection.pushConstantRanges) {
      pushConstantRanges.push_back(x);
    }
  }

  for (auto &[set, layout] : descriptorLayoutsMap) {
    descriptorLayoutsRaw.push_back(layout);
    mDescriptorLayouts.insert({set, layout});
  }

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.setLayoutCount =
      static_cast<uint32_t>(mDescriptorLayouts.size());
  pipelineLayoutCreateInfo.pSetLayouts = descriptorLayoutsRaw.data();
  pipelineLayoutCreateInfo.pushConstantRangeCount =
      static_cast<uint32_t>(pushConstantRanges.size());
  pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

  checkForVulkanError(vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo,
                                             nullptr, &mPipelineLayout),
                      "Failed to create pipeline layout");

  LOG_DEBUG_VK("Pipeline layout created. Descriptor layouts: "
                   << descriptorLayoutsRaw.size()
                   << "; Push constants: " << pushConstantRanges.size(),
               mPipelineLayout);

  // Dynamic state
  std::array<VkDynamicState, 2> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT,
                                              VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.flags = 0;
  dynamicState.pNext = nullptr;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // Viewport
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = nullptr;
  viewportState.scissorCount = 1;
  viewportState.pScissors = nullptr;

  // Input assembly
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VulkanMapping::getPrimitiveTopology(
      description.inputAssembly.primitiveTopology);
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.polygonMode =
      VulkanMapping::getPolygonMode(description.rasterizer.polygonMode);
  rasterizer.cullMode =
      VulkanMapping::getCullMode(description.rasterizer.cullMode);
  rasterizer.frontFace =
      VulkanMapping::getFrontFace(description.rasterizer.frontFace);
  rasterizer.lineWidth = description.rasterizer.lineWidth;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
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

  // depth stencil
  VkPipelineDepthStencilStateCreateInfo depthStencilState{};
  depthStencilState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilState.pNext = nullptr;
  depthStencilState.flags = 0;
  depthStencilState.depthTestEnable = VK_TRUE;
  depthStencilState.depthWriteEnable = VK_TRUE;
  depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencilState.depthBoundsTestEnable = VK_FALSE;
  depthStencilState.minDepthBounds = 0.0f;
  depthStencilState.maxDepthBounds = 1.0f;
  depthStencilState.stencilTestEnable = VK_FALSE;

  std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(
      description.colorBlend.attachments.size(),
      VkPipelineColorBlendAttachmentState{});

  for (size_t i = 0; i < description.colorBlend.attachments.size(); ++i) {
    const auto &src = description.colorBlend.attachments.at(i);
    auto &dst = colorBlendAttachments.at(i);

    dst.blendEnable = src.enabled;
    dst.srcColorBlendFactor = VulkanMapping::getBlendFactor(src.srcColor);
    dst.dstColorBlendFactor = VulkanMapping::getBlendFactor(src.dstColor);
    dst.colorBlendOp = VulkanMapping::getBlendOp(src.colorOp);
    dst.srcAlphaBlendFactor = VulkanMapping::getBlendFactor(src.srcAlpha);
    dst.dstAlphaBlendFactor = VulkanMapping::getBlendFactor(src.dstAlpha);
    dst.alphaBlendOp = VulkanMapping::getBlendOp(src.alphaOp);
    dst.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                         VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  }

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.pNext = nullptr;
  colorBlending.flags = 0;
  colorBlending.attachmentCount =
      static_cast<uint32_t>(colorBlendAttachments.size());
  colorBlending.pAttachments = colorBlendAttachments.data();
  colorBlending.logicOpEnable = false;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  // vertex input
  VkPipelineVertexInputStateCreateInfo vertexInput{};
  vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInput.flags = 0;
  vertexInput.pNext = nullptr;

  // Vertex Input Bindings
  vertexInput.vertexBindingDescriptionCount =
      static_cast<uint32_t>(description.inputLayout.bindings.size());
  std::vector<VkVertexInputBindingDescription> vertexInputBindings(
      description.inputLayout.bindings.size());
  for (size_t i = 0; i < description.inputLayout.bindings.size(); ++i) {
    vertexInputBindings.at(i) = VkVertexInputBindingDescription{
        description.inputLayout.bindings.at(i).binding,
        description.inputLayout.bindings.at(i).stride,
        VulkanMapping::getVertexInputRate(
            description.inputLayout.bindings.at(i).inputRate)};
  }
  vertexInput.pVertexBindingDescriptions = vertexInputBindings.data();

  vertexInput.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(description.inputLayout.attributes.size());
  std::vector<VkVertexInputAttributeDescription> vertexInputDescriptions(
      description.inputLayout.attributes.size());

  for (size_t i = 0; i < description.inputLayout.attributes.size(); ++i) {
    vertexInputDescriptions.at(i) = VkVertexInputAttributeDescription{
        description.inputLayout.attributes.at(i).slot,
        description.inputLayout.attributes.at(i).binding,
        VulkanMapping::getFormat(
            description.inputLayout.attributes.at(i).format),
        description.inputLayout.attributes.at(i).offset,
    };
  }

  vertexInput.pVertexAttributeDescriptions = vertexInputDescriptions.data();

  const auto &pass = registry.getRenderPasses().at(description.renderPass);
  // Pipeline info
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = nullptr;
  pipelineInfo.flags = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;
  pipelineInfo.renderPass = pass->getRenderPass();
  pipelineInfo.subpass = 0;
  pipelineInfo.layout = mPipelineLayout;
  pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
  pipelineInfo.pStages = stages.data();
  pipelineInfo.pVertexInputState = &vertexInput;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencilState;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;

  checkForVulkanError(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1,
                                                &pipelineInfo, nullptr,
                                                &mPipeline),
                      "Failed to create pipeline");

  LOG_DEBUG_VK("Pipeline created", mPipeline);
}

VulkanPipeline::~VulkanPipeline() {
  if (mPipeline) {
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    LOG_DEBUG_VK("Pipeline destroyed", mPipeline);
  }

  if (mPipelineLayout) {
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
    LOG_DEBUG_VK("Pipeline layout destroyed", mPipelineLayout);
  }

  for (auto &[s, x] : mDescriptorLayouts) {
    vkDestroyDescriptorSetLayout(mDevice, x, nullptr);
  }
}

} // namespace liquid::rhi
