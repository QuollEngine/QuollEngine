#include "core/Base.h"
#include "VulkanGraphEvaluator.h"
#include "VulkanTextureBinder.h"
#include "VulkanError.h"
#include "VulkanMapping.h"
#include "VulkanPipeline.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"

#include <vulkan/vulkan.hpp>

#include "core/EngineGlobals.h"

namespace liquid {

VulkanGraphEvaluator::VulkanGraphEvaluator(
    VulkanContext &vulkanInstance_, VulkanSwapchain &swapchain_,
    ResourceAllocator *resourceAllocator_)
    : vulkanInstance(vulkanInstance_), swapchain(swapchain_),
      resourceAllocator(resourceAllocator_) {}

std::vector<RenderGraphPassInterface *>
VulkanGraphEvaluator::build(RenderGraph &graph) {
  const auto &&passes = graph.compile();

  for (auto &pass : passes) {
    buildPass(pass, graph, false);
  }
  return passes;
}

void VulkanGraphEvaluator::rebuildSwapchainRelatedPasses(RenderGraph &graph) {
  for (auto &pass : graph.getRenderPasses()) {
    if (!pass->isSwapchainRelative()) {
      continue;
    }

    buildPass(pass, graph, true);
  }
}

void VulkanGraphEvaluator::execute(
    RenderCommandList &commandList,
    const std::vector<RenderGraphPassInterface *> &result, RenderGraph &graph,
    uint32_t imageIdx) {

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

    item->execute(commandList);
    commandList.endRenderPass();
  }
}

void VulkanGraphEvaluator::buildPass(RenderGraphPassInterface *pass,
                                     RenderGraph &graph, bool force) {
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkAttachmentReference> colorAttachments;
  std::optional<VkAttachmentReference> depthAttachment;
  std::vector<VkClearValue> clearValues;
  std::vector<std::vector<VkImageView>> framebufferAttachments(1);

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;

  for (auto &resourceId : pass->getOutputs()) {
    VulkanAttachmentInfo info{};

    if (!force &&
        graph.getResourceRegistry().hasRenderPass(pass->getRenderPass()) &&
        (graph.getResourceRegistry().hasTexture(resourceId) ||
         graph.isSwapchainColor(resourceId) ||
         graph.isSwapchainDepth(resourceId))) {
      continue;
    }

    if (graph.isSwapchainColor(resourceId)) {
      framebufferAttachments.resize(swapchain.getImageViews().size());
      info = createSwapchainColorAttachment(attachments.size());
      colorAttachments.push_back(info.reference);
    } else if (graph.isSwapchainDepth(resourceId)) {
      framebufferAttachments.resize(swapchain.getImageViews().size());
      info = createSwapchainDepthAttachment(attachments.size());
      depthAttachment = info.reference;
    } else {
      const auto &graphAttachment = graph.getAttachment(resourceId);
      if (graphAttachment.type == AttachmentType::Color) {
        info = createColorAttachment(graphAttachment, attachments.size());
        colorAttachments.push_back(info.reference);
        ;
      } else if (graphAttachment.type == AttachmentType::Depth) {
        info = createDepthAttachment(graphAttachment, attachments.size());
        depthAttachment = info.reference;
      } else {
        continue;
      }
    }

    clearValues.push_back(info.clearValue);
    attachments.push_back(info.description);

    for (size_t i = 0; i < info.framebufferAttachments.size(); ++i) {
      framebufferAttachments.at(i).push_back(info.framebufferAttachments.at(i));
    }

    width = info.width;
    height = info.height;
    layers = info.layers;

    if (info.texture) {
      graph.getResourceRegistry().addTexture(resourceId, info.texture);
    }
  }

  if (!colorAttachments.empty() || depthAttachment.has_value()) {
    VkRenderPass renderPass = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> framebuffers(framebufferAttachments.size(),
                                            VK_NULL_HANDLE);

    VkSubpassDescription subpass{};
    subpass.flags = 0;
    subpass.colorAttachmentCount = colorAttachments.size();
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
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = nullptr;

    checkForVulkanError(vkCreateRenderPass(vulkanInstance.getDevice(),
                                           &createInfo, nullptr, &renderPass),
                        "Failed to create render pass");

    LOG_DEBUG("[Vulkan] Render pass created");

    for (size_t i = 0; i < framebuffers.size(); ++i) {
      VkFramebufferCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      createInfo.flags = 0;
      createInfo.pNext = nullptr;
      createInfo.pAttachments = framebufferAttachments.at(i).data();
      createInfo.attachmentCount = framebufferAttachments.at(i).size();
      createInfo.renderPass = renderPass;
      createInfo.width = width;
      createInfo.height = height;
      createInfo.layers = layers;

      checkForVulkanError(vkCreateFramebuffer(vulkanInstance.getDevice(),
                                              &createInfo, nullptr,
                                              &framebuffers.at(i)),
                          "Failed to create framebuffer");
    }

    if (framebuffers.size() > 0) {
      LOG_DEBUG("[Vulkan] Framebuffers created");
    }

    graph.getResourceRegistry().addRenderPass(
        pass->getRenderPass(),
        std::make_shared<VulkanRenderPass>(vulkanInstance.getDevice(),
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
VulkanGraphEvaluator::createSwapchainColorAttachment(uint32_t index) {
  constexpr glm::vec4 blueishClearValue{0.19f, 0.21f, 0.26f, 1.0f};

  VulkanAttachmentInfo info{};

  // Attachment description
  info.description.format = swapchain.getSurfaceFormat().format;
  info.description.samples = VK_SAMPLE_COUNT_1_BIT;
  info.description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  info.description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  info.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  info.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  info.description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Attachment reference
  info.reference.attachment = index;
  info.reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Attachment clear value
  info.clearValue.color.float32[0] = blueishClearValue.r;
  info.clearValue.color.float32[1] = blueishClearValue.g;
  info.clearValue.color.float32[2] = blueishClearValue.b;
  info.clearValue.color.float32[3] = blueishClearValue.a;

  // Framebuffer image views
  info.framebufferAttachments.resize(swapchain.getImageViews().size(),
                                     VK_NULL_HANDLE);

  for (size_t i = 0; i < swapchain.getImageViews().size(); ++i) {
    info.framebufferAttachments.at(i) = swapchain.getImageViews().at(i);
  }

  // Dimensions
  info.width = swapchain.getExtent().width;
  info.height = swapchain.getExtent().height;
  info.layers = 1;

  return info;
}

VulkanGraphEvaluator::VulkanAttachmentInfo
VulkanGraphEvaluator::createSwapchainDepthAttachment(uint32_t index) {
  VulkanAttachmentInfo info{};

  // Attachment description
  info.description.format = swapchain.getDepthFormat();
  info.description.samples = VK_SAMPLE_COUNT_1_BIT;
  info.description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  info.description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  info.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  info.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  info.description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.description.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Attachment reference
  info.reference.attachment = index;
  info.reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Attachment clear value
  info.clearValue.depthStencil.depth = 1.0f;
  info.clearValue.depthStencil.stencil = 0;

  // Framebuffer image views
  info.framebufferAttachments.resize(swapchain.getImageViews().size(),
                                     VK_NULL_HANDLE);

  for (size_t i = 0; i < swapchain.getImageViews().size(); ++i) {
    info.framebufferAttachments.at(i) = swapchain.getDepthImageView();
  }

  // Dimensions
  info.width = swapchain.getExtent().width;
  info.height = swapchain.getExtent().height;
  info.layers = 1;

  return info;
}

VulkanGraphEvaluator::VulkanAttachmentInfo
VulkanGraphEvaluator::createColorAttachment(
    const RenderPassAttachment &attachment, uint32_t index) {
  VulkanAttachmentInfo info{};

  // Attachment description
  info.description.format =
      static_cast<VkFormat>(attachment.textureData.format);
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
  VkAttachmentReference reference{};
  reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  reference.attachment = index;

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
  info.texture =
      resourceAllocator->createTextureFramebuffer(attachment.textureData);

  info.framebufferAttachments.push_back(
      std::dynamic_pointer_cast<VulkanTextureBinder>(
          info.texture->getResourceBinder())
          ->getImageView());

  // Dimensions
  info.width = attachment.textureData.width;
  info.height = attachment.textureData.height;
  info.layers = attachment.textureData.layers;

  return info;
}

VulkanGraphEvaluator::VulkanAttachmentInfo
VulkanGraphEvaluator::createDepthAttachment(
    const RenderPassAttachment &attachment, uint32_t index) {
  VulkanAttachmentInfo info{};

  // Attachment description
  info.description.format =
      static_cast<VkFormat>(attachment.textureData.format);
  info.description.samples = VK_SAMPLE_COUNT_1_BIT;
  info.description.loadOp =
      VulkanMapping::getAttachmentLoadOp(attachment.loadOp);
  info.description.storeOp =
      VulkanMapping::getAttachmentStoreOp(attachment.storeOp);
  info.description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  info.description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  info.description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.description.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

  // Attachment reference
  info.reference.attachment = index;
  info.reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Attachment clear value
  info.clearValue.depthStencil.depth =
      std::get<DepthStencilClear>(attachment.clearValue).clearDepth;
  info.clearValue.depthStencil.stencil =
      std::get<DepthStencilClear>(attachment.clearValue).clearStencil;

  // Framebuffer image views
  info.texture =
      resourceAllocator->createTextureFramebuffer(attachment.textureData);

  info.framebufferAttachments.push_back(
      std::dynamic_pointer_cast<VulkanTextureBinder>(
          info.texture->getResourceBinder())
          ->getImageView());

  // Dimensions
  info.width = attachment.textureData.width;
  info.height = attachment.textureData.height;
  info.layers = attachment.textureData.layers;

  return info;
}

const SharedPtr<Pipeline> VulkanGraphEvaluator::createGraphicsPipeline(
    const PipelineDescriptor &descriptor, VkRenderPass renderPass) {
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
  std::vector<VkDescriptorSetLayout> descriptorLayouts;
  std::vector<VkPushConstantRange> pushConstantRanges;

  for (auto &shader : shaders) {
    const auto &reflection = shader->getReflectionData();
    for (auto &x : reflection.descriptorSetLayouts) {

      std::vector<VkDescriptorBindingFlags> bindingFlags(
          x.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

      VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{};
      bindingFlagsCreateInfo.sType =
          VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
      bindingFlagsCreateInfo.pNext = nullptr;
      bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();
      bindingFlagsCreateInfo.bindingCount = bindingFlags.size();

      VkDescriptorSetLayout layout = VK_NULL_HANDLE;
      VkDescriptorSetLayoutCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      createInfo.flags = 0;
      createInfo.pNext = &bindingFlagsCreateInfo;
      createInfo.bindingCount = x.size();
      createInfo.pBindings = x.data();
      checkForVulkanError(
          vkCreateDescriptorSetLayout(vulkanInstance.getDevice(), &createInfo,
                                      nullptr, &layout),
          "Failed to create descriptor set layout");

      descriptorLayouts.push_back(layout);
    }

    for (auto &x : reflection.pushConstantRanges) {
      pushConstantRanges.push_back(x);
    }
  }

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorLayouts.size());
  pipelineLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();
  pipelineLayoutCreateInfo.pushConstantRangeCount =
      static_cast<uint32_t>(pushConstantRanges.size());
  pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  checkForVulkanError(vkCreatePipelineLayout(vulkanInstance.getDevice(),
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
  colorBlending.attachmentCount = colorBlendAttachments.size();
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
      descriptor.inputLayout.bindings.size();
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
      descriptor.inputLayout.attributes.size();
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
  pipelineInfo.stageCount = stages.size();
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
      vkCreateGraphicsPipelines(vulkanInstance.getDevice(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr, &pipeline),
      "Failed to create pipeline");

  LOG_DEBUG("[Vulkan] Pipeline created");

  return std::make_shared<VulkanPipeline>(vulkanInstance.getDevice(), pipeline,
                                          pipelineLayout, descriptorLayouts);
}

} // namespace liquid
