#include "liquid/core/Base.h"
#include "VulkanGraphEvaluator.h"
#include "VulkanError.h"
#include "VulkanMapping.h"
#include "VulkanPipeline.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "liquid/rhi/vulkan/VulkanTexture.h"

#include <vulkan/vulkan.hpp>

#include "liquid/core/EngineGlobals.h"

namespace liquid {

VulkanGraphEvaluator::VulkanGraphEvaluator(
    experimental::VulkanRenderDevice *device_,
    experimental::ResourceRegistry &registry_,
    const experimental::VulkanResourceRegistry &realRegistry_)
    : device(device_), registry(registry_), realRegistry(realRegistry_) {}

std::vector<RenderGraphPassBase *>
VulkanGraphEvaluator::compile(RenderGraph &graph, bool swapchainRecreated,
                              const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::compile");

  const auto &&compiled = graph.compile();

  for (const auto &[resourceId, textureDesc] : graph.getTextures()) {
    if (!graph.getResourceRegistry().hasTexture(resourceId) ||
        (swapchainRecreated &&
         textureDesc.sizeMethod == AttachmentSizeMethod::SwapchainRelative)) {
      graph.getResourceRegistry().addTexture(
          resourceId, createTexture(textureDesc, extent));
    }
  }

  return compiled;
}

void VulkanGraphEvaluator::build(std::vector<RenderGraphPassBase *> &compiled,
                                 RenderGraph &graph, bool swapchainRecreated,
                                 uint32_t numSwapchainImages,
                                 const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::build");

  for (auto &pass : compiled) {
    buildPass(pass, graph, swapchainRecreated && pass->isSwapchainRelative(),
              numSwapchainImages, extent);
  }
}

void VulkanGraphEvaluator::execute(
    RenderCommandList &commandList,
    const std::vector<RenderGraphPassBase *> &result, RenderGraph &graph,
    uint32_t imageIdx) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::execute");

  for (auto &item : result) {
    const auto &renderPass = std::static_pointer_cast<VulkanRenderPass>(
        graph.getResourceRegistry().getRenderPass(item->getRenderPass()));

    commandList.beginRenderPass(
        renderPass->getRenderPass(),
        renderPass->getFramebuffers().at(imageIdx %
                                         renderPass->getFramebuffers().size()),
        {0, 0}, renderPass->getExtent(), renderPass->getClearValues());
    commandList.setViewport({0.0f, 0.0f}, renderPass->getExtent(),
                            {0.0f, 1.0f});
    commandList.setScissor({0.0f, 0.0f}, renderPass->getExtent());

    item->execute(commandList, graph.getResourceRegistry());
    commandList.endRenderPass();
  }
}

void VulkanGraphEvaluator::buildPass(RenderGraphPassBase *pass,
                                     RenderGraph &graph, bool force,
                                     uint32_t numSwapchainImages,
                                     const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::buildPass");
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkAttachmentReference> colorAttachments;
  std::optional<VkAttachmentReference> depthAttachment;
  std::vector<VkClearValue> clearValues;

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;

  size_t imageViewsPerFramebuffer = 1;

  for (auto &[resourceId, _] : pass->getOutputs()) {
    if (graph.isSwapchain(resourceId)) {
      imageViewsPerFramebuffer = numSwapchainImages;
      break;
    }
  }

  std::vector<std::vector<VkImageView>> framebufferAttachments(
      imageViewsPerFramebuffer);

  for (auto &[resourceId, graphAttachment] : pass->getOutputs()) {
    VulkanAttachmentInfo info{};

    if (!force &&
        graph.getResourceRegistry().hasRenderPass(pass->getRenderPass()) &&
        (graph.getResourceRegistry().hasTexture(resourceId) ||
         graph.isSwapchain(resourceId))) {
      continue;
    }

    uint32_t lastAttachmentIndex = static_cast<uint32_t>(attachments.size());

    if (graph.isSwapchain(resourceId)) {
      info = createSwapchainAttachment(graphAttachment, lastAttachmentIndex,
                                       numSwapchainImages, extent);
      colorAttachments.push_back(info.reference);
    } else {
      const auto &texture = graph.getResourceRegistry().getTexture(resourceId);
      const auto &textureType = graph.getTextures().at(resourceId).type;
      if (textureType == AttachmentType::Color) {
        info = createColorAttachment(graphAttachment, texture,
                                     lastAttachmentIndex);
        colorAttachments.push_back(info.reference);
      } else if (textureType == AttachmentType::Depth) {
        info = createDepthAttachment(graphAttachment, texture,
                                     lastAttachmentIndex);
        depthAttachment = info.reference;
      }
    }

    clearValues.push_back(info.clearValue);
    attachments.push_back(info.description);

    for (size_t i = 0; i < framebufferAttachments.size(); ++i) {
      framebufferAttachments.at(i).push_back(info.framebufferAttachments.at(
          i % info.framebufferAttachments.size()));
    }

    width = info.width;
    height = info.height;
    layers = info.layers;
  }

  if (!colorAttachments.empty() || depthAttachment.has_value()) {
    VkRenderPass renderPass = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> framebuffers(framebufferAttachments.size(),
                                            VK_NULL_HANDLE);

    VkSubpassDescription subpass{};
    subpass.flags = 0;
    subpass.colorAttachmentCount =
        static_cast<uint32_t>(colorAttachments.size());
    subpass.pColorAttachments = colorAttachments.data();
    subpass.pDepthStencilAttachment =
        depthAttachment.has_value() ? &depthAttachment.value() : nullptr;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pResolveAttachments = nullptr;

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = nullptr;

    checkForVulkanError(vkCreateRenderPass(device->getVulkanDevice(),
                                           &createInfo, nullptr, &renderPass),
                        "Failed to create render pass");

    LOG_DEBUG("[Vulkan] Render pass created");

    for (size_t i = 0; i < framebuffers.size(); ++i) {
      VkFramebufferCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      createInfo.flags = 0;
      createInfo.pNext = nullptr;
      createInfo.pAttachments = framebufferAttachments.at(i).data();
      createInfo.attachmentCount =
          static_cast<uint32_t>(framebufferAttachments.at(i).size());
      createInfo.renderPass = renderPass;
      createInfo.width = width;
      createInfo.height = height;
      createInfo.layers = layers;

      checkForVulkanError(vkCreateFramebuffer(device->getVulkanDevice(),
                                              &createInfo, nullptr,
                                              &framebuffers.at(i)),
                          "Failed to create framebuffer");
    }

    if (framebuffers.size() > 0) {
      LOG_DEBUG("[Vulkan] Framebuffers created");
    }

    graph.getResourceRegistry().addRenderPass(
        pass->getRenderPass(),
        std::make_shared<VulkanRenderPass>(device->getVulkanDevice(),
                                           renderPass, framebuffers,
                                           clearValues, width, height, layers));
  }

  LIQUID_ASSERT(
      graph.getResourceRegistry().hasRenderPass(pass->getRenderPass()),
      "Render pass with ID " + std::to_string(pass->getRenderPass()) +
          " does not exist and is not created");

  VkRenderPass renderPass =
      std::dynamic_pointer_cast<VulkanRenderPass>(
          graph.getResourceRegistry().getRenderPass(pass->getRenderPass()))
          ->getRenderPass();

  for (auto resource : pass->getResources()) {
    if (graph.isPipeline(resource)) {
      if (!force && graph.getResourceRegistry().hasPipeline(resource)) {
        continue;
      }

      const auto &pipeline =
          createGraphicsPipeline(graph.getPipeline(resource), renderPass);
      graph.getResourceRegistry().addPipeline(resource, pipeline);
    }
  }
}

VulkanGraphEvaluator::VulkanAttachmentInfo
VulkanGraphEvaluator::createSwapchainAttachment(
    const RenderPassAttachment &attachment, uint32_t index,
    uint32_t numSwapchainImages, const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::createSwapchainAttachment");
  LIQUID_ASSERT(numSwapchainImages > 0,
                "Number of swapchain images must be greater than ZERO");
  VulkanAttachmentInfo info{};

  // Attachment description
  info.description.format = realRegistry.getTexture(1)->getFormat();
  info.description.samples = VK_SAMPLE_COUNT_1_BIT;
  info.description.loadOp =
      VulkanMapping::getAttachmentLoadOp(attachment.loadOp);
  info.description.storeOp =
      VulkanMapping::getAttachmentStoreOp(attachment.storeOp);
  info.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  info.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  info.description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Attachment reference
  info.reference.attachment = index;
  info.reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Attachment clear value
  info.clearValue.color.float32[0] =
      std::get<glm::vec4>(attachment.clearValue).x;
  info.clearValue.color.float32[1] =
      std::get<glm::vec4>(attachment.clearValue).y;
  info.clearValue.color.float32[2] =
      std::get<glm::vec4>(attachment.clearValue).z;
  info.clearValue.color.float32[3] =
      std::get<glm::vec4>(attachment.clearValue).w;

  // Framebuffer image views
  info.framebufferAttachments.resize(numSwapchainImages, VK_NULL_HANDLE);

  for (uint32_t i = 0; i < numSwapchainImages; ++i) {
    auto handle = static_cast<TextureHandle>(i + 1);

    info.framebufferAttachments.at(i) =
        realRegistry.getTexture(handle)->getImageView();
  }

  // Dimensions
  info.width = extent.x;
  info.height = extent.y;
  info.layers = 1;

  return info;
}

VulkanGraphEvaluator::VulkanAttachmentInfo
VulkanGraphEvaluator::createColorAttachment(
    const RenderPassAttachment &attachment, TextureHandle texture,
    uint32_t index) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::createColorAttachment");
  VulkanAttachmentInfo info{};

  const auto &desc = registry.getTextureMap().getDescription(texture);

  // Attachment description
  info.description.format = static_cast<VkFormat>(desc.format);
  info.description.samples = VK_SAMPLE_COUNT_1_BIT;
  info.description.loadOp =
      VulkanMapping::getAttachmentLoadOp(attachment.loadOp);
  info.description.storeOp =
      VulkanMapping::getAttachmentStoreOp(attachment.storeOp);
  info.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  info.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  info.description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Attachment reference
  info.reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  info.reference.attachment = index;

  // Attachment clear value
  info.clearValue.color.float32[0] =
      std::get<glm::vec4>(attachment.clearValue).x;
  info.clearValue.color.float32[1] =
      std::get<glm::vec4>(attachment.clearValue).y;
  info.clearValue.color.float32[2] =
      std::get<glm::vec4>(attachment.clearValue).z;
  info.clearValue.color.float32[3] =
      std::get<glm::vec4>(attachment.clearValue).w;

  info.framebufferAttachments.push_back(
      realRegistry.getTexture(texture)->getImageView());

  // Dimensions
  info.width = desc.width;
  info.height = desc.height;
  info.layers = desc.layers;

  return info;
}

VulkanGraphEvaluator::VulkanAttachmentInfo
VulkanGraphEvaluator::createDepthAttachment(
    const RenderPassAttachment &attachment, TextureHandle texture,
    uint32_t index) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::createDepthAttachment");
  VulkanAttachmentInfo info{};

  const auto &desc = registry.getTextureMap().getDescription(texture);

  // Attachment description
  info.description.format = static_cast<VkFormat>(desc.format);
  info.description.samples = VK_SAMPLE_COUNT_1_BIT;
  info.description.loadOp =
      VulkanMapping::getAttachmentLoadOp(attachment.loadOp);
  info.description.storeOp =
      VulkanMapping::getAttachmentStoreOp(attachment.storeOp);
  info.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  info.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  info.description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.description.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Attachment reference
  info.reference.attachment = index;
  info.reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Attachment clear value
  info.clearValue.depthStencil.depth =
      std::get<DepthStencilClear>(attachment.clearValue).clearDepth;
  info.clearValue.depthStencil.stencil =
      std::get<DepthStencilClear>(attachment.clearValue).clearStencil;

  info.framebufferAttachments.push_back(
      realRegistry.getTexture(texture)->getImageView());

  // Dimensions
  info.width = desc.width;
  info.height = desc.height;
  info.layers = desc.layers;

  return info;
}

const SharedPtr<Pipeline> VulkanGraphEvaluator::createGraphicsPipeline(
    const PipelineDescriptor &descriptor, VkRenderPass renderPass) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::createGraphicsPipeline");

  VkPipeline pipeline = VK_NULL_HANDLE;

  std::array<SharedPtr<VulkanShader>, 2> shaders{
      std::dynamic_pointer_cast<VulkanShader>(descriptor.vertexShader),
      std::dynamic_pointer_cast<VulkanShader>(descriptor.fragmentShader),
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
  std::unordered_map<uint32_t, VkDescriptorSetLayout> descriptorLayouts;
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
      checkForVulkanError(vkCreateDescriptorSetLayout(device->getVulkanDevice(),
                                                      &createInfo, nullptr,
                                                      &layout),
                          "Failed to create descriptor set layout");

      descriptorLayoutsMap.insert({set, layout});
    }

    for (auto &x : reflection.pushConstantRanges) {
      pushConstantRanges.push_back(x);
    }
  }

  for (auto &[set, layout] : descriptorLayoutsMap) {
    descriptorLayoutsRaw.push_back(layout);
    descriptorLayouts.insert({set, layout});
  }

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorLayouts.size());
  pipelineLayoutCreateInfo.pSetLayouts = descriptorLayoutsRaw.data();
  pipelineLayoutCreateInfo.pushConstantRangeCount =
      static_cast<uint32_t>(pushConstantRanges.size());
  pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  checkForVulkanError(vkCreatePipelineLayout(device->getVulkanDevice(),
                                             &pipelineLayoutCreateInfo, nullptr,
                                             &pipelineLayout),
                      "Failed to create pipeline layout");

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
      descriptor.inputAssembly.primitiveTopology);
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.polygonMode =
      VulkanMapping::getPolygonMode(descriptor.rasterizer.polygonMode);
  rasterizer.cullMode =
      VulkanMapping::getCullMode(descriptor.rasterizer.cullMode);
  rasterizer.frontFace =
      VulkanMapping::getFrontFace(descriptor.rasterizer.frontFace);
  rasterizer.lineWidth = 1.0f;
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

  std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(
      descriptor.colorBlend.attachments.size(),
      VkPipelineColorBlendAttachmentState{});

  for (size_t i = 0; i < descriptor.colorBlend.attachments.size(); ++i) {
    const auto &src = descriptor.colorBlend.attachments.at(i);
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
      static_cast<uint32_t>(descriptor.inputLayout.bindings.size());
  std::vector<VkVertexInputBindingDescription> vertexInputBindings(
      descriptor.inputLayout.bindings.size());
  for (size_t i = 0; i < descriptor.inputLayout.bindings.size(); ++i) {
    vertexInputBindings.at(i) = VkVertexInputBindingDescription{
        descriptor.inputLayout.bindings.at(i).binding,
        descriptor.inputLayout.bindings.at(i).stride,
        VulkanMapping::getVertexInputRate(
            descriptor.inputLayout.bindings.at(i).inputRate)};
  }
  vertexInput.pVertexBindingDescriptions = vertexInputBindings.data();

  vertexInput.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(descriptor.inputLayout.attributes.size());
  std::vector<VkVertexInputAttributeDescription> vertexInputDescriptions(
      descriptor.inputLayout.attributes.size());

  for (size_t i = 0; i < descriptor.inputLayout.attributes.size(); ++i) {
    vertexInputDescriptions.at(i) = VkVertexInputAttributeDescription{
        descriptor.inputLayout.attributes.at(i).slot,
        descriptor.inputLayout.attributes.at(i).binding,
        static_cast<VkFormat>(descriptor.inputLayout.attributes.at(i).format),
        descriptor.inputLayout.attributes.at(i).offset,
    };
  }

  vertexInput.pVertexAttributeDescriptions = vertexInputDescriptions.data();

  // Pipeline info
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = nullptr;
  pipelineInfo.flags = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.layout = pipelineLayout;
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

  checkForVulkanError(
      vkCreateGraphicsPipelines(device->getVulkanDevice(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr, &pipeline),
      "Failed to create pipeline");

  LOG_DEBUG("[Vulkan] Pipeline created");

  return std::make_shared<VulkanPipeline>(device->getVulkanDevice(), pipeline,
                                          pipelineLayout, descriptorLayouts);
}

TextureHandle VulkanGraphEvaluator::createTexture(const AttachmentData &data,
                                                  const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("VulkanGraphEvaluator::createTexture");
  uint32_t width = data.width;
  uint32_t height = data.height;

  if (data.sizeMethod == AttachmentSizeMethod::SwapchainRelative) {
    constexpr uint32_t PERCENTAGE_RATIO = 100;
    width = extent.x * width / PERCENTAGE_RATIO;
    height = extent.y * height / PERCENTAGE_RATIO;
  }

  TextureDescription description;
  description.width = width;
  description.height = height;
  description.layers = data.layers;
  description.format = data.format;
  if (data.type == AttachmentType::Color) {
    description.usageFlags =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    description.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
  } else if (data.type == AttachmentType::Depth) {
    description.usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                             VK_IMAGE_USAGE_SAMPLED_BIT;
    description.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
  }

  return registry.addTexture(description);
}

} // namespace liquid
