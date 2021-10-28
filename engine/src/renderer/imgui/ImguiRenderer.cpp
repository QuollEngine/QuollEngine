#include "core/Base.h"
#include "core/EngineGlobals.h"

#include "ImguiRenderer.h"
#include <imgui_impl_glfw.h>

#include "ImguiError.h"

static const VkDeviceSize BUFFER_MEMORY_ALIGNMENT = 256;

/**
 * @brief Aligns given size based on specified alignment
 *
 * @param size Size to align
 * @return Aligned size
 */
static inline VkDeviceSize getAlignedBufferSize(VkDeviceSize size) {
  return ((size - 1) / BUFFER_MEMORY_ALIGNMENT + 1) * BUFFER_MEMORY_ALIGNMENT;
}

namespace liquid {

ImguiRenderer::ImguiRenderer(GLFWWindow *window,
                             const VulkanContext &vulkanContext_,
                             VkRenderPass renderPass_,
                             ShaderLibrary *shaderLibrary_,
                             ResourceAllocator *resourceAllocator_)
    : vulkanContext(vulkanContext_), resourceAllocator(resourceAllocator_),
      shaderLibrary(shaderLibrary_), renderPass(renderPass_) {
  ImGui::CreateContext();

  descriptorManager = new VulkanDescriptorManager(vulkanContext.getDevice());
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(window->getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "ImguiCustomBackend";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

  frameData.resize(2);
  loadFonts();
  createDescriptorLayout();
  createPipeline();

  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");
}

ImguiRenderer::~ImguiRenderer() {
  delete descriptorManager;

  fontTexture = nullptr;
  frameData.clear();

  if (descriptorLayout) {
    vkDestroyDescriptorSetLayout(vulkanContext.getDevice(), descriptorLayout,
                                 nullptr);
    descriptorLayout = VK_NULL_HANDLE;
  }

  if (pipelineLayout) {
    vkDestroyPipelineLayout(vulkanContext.getDevice(), pipelineLayout, nullptr);
    pipelineLayout = VK_NULL_HANDLE;
  }

  if (pipeline) {
    vkDestroyPipeline(vulkanContext.getDevice(), pipeline, nullptr);
    pipeline = VK_NULL_HANDLE;
  }

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() { ImGui::Render(); }

void ImguiRenderer::draw(RenderCommandList &commandList) {
  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);
  if (fbWidth <= 0 || fbHeight <= 0)
    return;

  currentFrame = (currentFrame + 1) % frameData.size();
  auto &frameObj = frameData.at(currentFrame);

  if (data->TotalVtxCount > 0) {
    size_t vertexSize = data->TotalVtxCount * sizeof(ImDrawVert);
    size_t indexSize = data->TotalIdxCount * sizeof(ImDrawIdx);

    if (!frameObj.vertexBuffer ||
        frameObj.vertexBuffer->getBufferSize() < vertexSize) {
      frameObj.vertexBuffer = resourceAllocator->createVertexBuffer(
          getAlignedBufferSize(vertexSize));
    }

    if (!frameObj.indexBuffer ||
        frameObj.indexBuffer->getBufferSize() < indexSize) {
      frameObj.indexBuffer =
          resourceAllocator->createIndexBuffer(getAlignedBufferSize(indexSize));
    }

    auto *vbDst = static_cast<ImDrawVert *>(frameObj.vertexBuffer->map());
    auto *ibDst = static_cast<ImDrawIdx *>(frameObj.indexBuffer->map());

    for (int n = 0; n < data->CmdListsCount; n++) {
      const ImDrawList *cmd_list = data->CmdLists[n];
      memcpy(vbDst, cmd_list->VtxBuffer.Data,
             cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
      memcpy(ibDst, cmd_list->IdxBuffer.Data,
             cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
      vbDst += cmd_list->VtxBuffer.Size;
      ibDst += cmd_list->IdxBuffer.Size;
    }

    frameObj.indexBuffer->unmap();
    frameObj.vertexBuffer->unmap();
  }

  setupRenderStates(data, commandList, fbWidth, fbHeight);

  uint32_t indexOffset = 0;
  uint32_t vertexOffset = 0;
  for (int cmdListIdx = 0; cmdListIdx < data->CmdListsCount; ++cmdListIdx) {
    const ImDrawList *cmdList = data->CmdLists[cmdListIdx];
    for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; ++cmdIdx) {
      const ImDrawCmd *cmd = &cmdList->CmdBuffer[cmdIdx];
      if (cmd->UserCallback != NULL) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (cmd->UserCallback == ImDrawCallback_ResetRenderState) {
          setupRenderStates(data, commandList, fbWidth, fbHeight);
        } else {
          cmd->UserCallback(cmdList, cmd);
        }
      } else {
        glm::vec2 clipRectOffset;
        glm::vec4 clipRectSize;

        clipRectOffset.x =
            (cmd->ClipRect.x - data->DisplayPos.x) * data->FramebufferScale.x;
        clipRectOffset.y =
            (cmd->ClipRect.y - data->DisplayPos.y) * data->FramebufferScale.y;
        clipRectSize.x =
            (cmd->ClipRect.z - data->DisplayPos.x) * data->FramebufferScale.x;
        clipRectSize.y =
            (cmd->ClipRect.w - data->DisplayPos.y) * data->FramebufferScale.y;

        if (clipRectOffset.x < (float)fbWidth &&
            clipRectOffset.y < (float)fbHeight && clipRectSize.x >= 0.0f &&
            clipRectSize.y >= 0.0f) {

          if (clipRectOffset.x < 0.0f) {
            clipRectOffset.x = 0.0f;
          }
          if (clipRectOffset.y < 0.0f) {
            clipRectOffset.y = 0.0f;
          }

          commandList.setScissor(clipRectOffset, clipRectSize);
          commandList.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

          auto *texture = static_cast<Texture *>(cmd->TextureId);

          if (descriptorMap.find(texture) == descriptorMap.end()) {
            descriptorMap.insert(
                {texture, createDescriptorFromTexture(texture)});
          }

          commandList.bindDescriptorSets(pipelineLayout,
                                         VK_PIPELINE_BIND_POINT_GRAPHICS, 0,
                                         {descriptorMap.at(texture)}, {});
          commandList.drawIndexed(
              cmd->ElemCount, cmd->IdxOffset + indexOffset,
              static_cast<int32_t>(cmd->VtxOffset + vertexOffset));
        }
      }
    }
    indexOffset += cmdList->IdxBuffer.Size;
    vertexOffset += cmdList->VtxBuffer.Size;
  }
}

void ImguiRenderer::createDescriptorLayout() {
  VkDescriptorSetLayoutBinding binding{};
  binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  std::array<VkDescriptorSetLayoutBinding, 1> bindings{binding};

  VkDescriptorSetLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = bindings.size();
  info.pBindings = bindings.data();
  vkCreateDescriptorSetLayout(vulkanContext.getDevice(), &info, nullptr,
                              &descriptorLayout);
}

VkDescriptorSet ImguiRenderer::createDescriptorFromTexture(Texture *texture) {
  const auto &binder = std::dynamic_pointer_cast<VulkanTextureBinder>(
      texture->getResourceBinder());

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorManager->getDescriptorPool();
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &descriptorLayout;
  vkAllocateDescriptorSets(vulkanContext.getDevice(), &allocInfo,
                           &descriptorSet);

  VkDescriptorImageInfo imageInfo{};
  imageInfo.sampler = binder->getSampler();
  imageInfo.imageView = binder->getImageView();
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  std::array<VkWriteDescriptorSet, 1> writes{};
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].dstSet = descriptorSet;
  writes[0].descriptorCount = 1;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writes[0].pImageInfo = &imageInfo;
  vkUpdateDescriptorSets(vulkanContext.getDevice(), writes.size(),
                         writes.data(), 0, NULL);

  return descriptorSet;
}

void ImguiRenderer::createPipeline() {
  std::array<VkPushConstantRange, 1> pushConstants{};
  pushConstants.at(0).stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstants.at(0).offset = 0;
  pushConstants.at(0).size = sizeof(float) * 4;

  std::array<VkDescriptorSetLayout, 1> setLayouts{descriptorLayout};
  VkPipelineLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutInfo.setLayoutCount = 1;
  layoutInfo.pSetLayouts = setLayouts.data();
  layoutInfo.pushConstantRangeCount = 1;
  layoutInfo.pPushConstantRanges = pushConstants.data();
  vkCreatePipelineLayout(vulkanContext.getDevice(), &layoutInfo, nullptr,
                         &pipelineLayout);

  const auto &vertexShader =
      std::dynamic_pointer_cast<VulkanShader>(
          shaderLibrary->getShader("__engine.imgui.vertex"))
          ->getShaderModule();
  const auto &fragmentShader =
      std::dynamic_pointer_cast<VulkanShader>(
          shaderLibrary->getShader("__engine.imgui.fragment"))
          ->getShaderModule();

  std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
  stages.at(0).sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stages.at(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
  stages.at(0).module = vertexShader;
  stages.at(0).pName = "main";
  stages.at(1).sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stages.at(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  stages.at(1).module = fragmentShader;
  stages.at(1).pName = "main";

  VkVertexInputBindingDescription bindingDescriptions{};
  bindingDescriptions.stride = sizeof(ImDrawVert);
  bindingDescriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
  attributeDescriptions.at(0).location = 0;
  attributeDescriptions.at(0).binding = 0;
  attributeDescriptions.at(0).format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions.at(0).offset = IM_OFFSETOF(ImDrawVert, pos);
  attributeDescriptions.at(1).location = 1;
  attributeDescriptions.at(1).binding = 0;
  attributeDescriptions.at(1).format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions.at(1).offset = IM_OFFSETOF(ImDrawVert, uv);
  attributeDescriptions.at(2).location = 2;
  attributeDescriptions.at(2).binding = 0;
  attributeDescriptions.at(2).format = VK_FORMAT_R8G8B8A8_UNORM;
  attributeDescriptions.at(2).offset = IM_OFFSETOF(ImDrawVert, col);

  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  vertexInputState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputState.vertexBindingDescriptionCount = 1;
  vertexInputState.pVertexBindingDescriptions = &bindingDescriptions;
  vertexInputState.vertexAttributeDescriptionCount = 3;
  vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkPipelineViewportStateCreateInfo viewport{};
  viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport.viewportCount = 1;
  viewport.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterization{};
  rasterization.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization.cullMode = VK_CULL_MODE_NONE;
  rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterization.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisample{};
  multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorAttachment{};
  colorAttachment.blendEnable = true;
  colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineDepthStencilStateCreateInfo depthInfo{};
  depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

  VkPipelineColorBlendStateCreateInfo blendInfo{};
  blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  blendInfo.attachmentCount = 1;
  blendInfo.pAttachments = &colorAttachment;

  std::array<VkDynamicState, 2> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT,
                                              VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = dynamicStates.size();
  dynamicState.pDynamicStates = dynamicStates.data();

  VkGraphicsPipelineCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.stageCount = stages.size();
  createInfo.pStages = stages.data();
  createInfo.pVertexInputState = &vertexInputState;
  createInfo.pInputAssemblyState = &inputAssembly;
  createInfo.pViewportState = &viewport;
  createInfo.pRasterizationState = &rasterization;
  createInfo.pMultisampleState = &multisample;
  createInfo.pDepthStencilState = &depthInfo;
  createInfo.pColorBlendState = &blendInfo;
  createInfo.pDynamicState = &dynamicState;
  createInfo.layout = pipelineLayout;
  createInfo.renderPass = renderPass;
  createInfo.subpass = 0;
  vkCreateGraphicsPipelines(vulkanContext.getDevice(), VK_NULL_HANDLE, 1,
                            &createInfo, nullptr, &pipeline);
}

void ImguiRenderer::setupRenderStates(ImDrawData *data,
                                      RenderCommandList &commandList,
                                      int fbWidth, int fbHeight) {
  if (data->TotalVtxCount > 0) {
    commandList.bindVertexBuffer(frameData.at(currentFrame).vertexBuffer);
    commandList.bindIndexBuffer(frameData.at(currentFrame).indexBuffer,
                                sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16
                                                       : VK_INDEX_TYPE_UINT32);
  }

  commandList.setViewport({0, 0}, {fbWidth, fbHeight}, {0.0f, 1.0f});

  const float SCALE_FACTOR = 2.0f;
  std::array<float, 2> scale{SCALE_FACTOR / data->DisplaySize.x,
                             SCALE_FACTOR / data->DisplaySize.y};
  std::array<float, 2> translate{-1.0f - data->DisplayPos.x * scale[0],
                                 -1.0f - data->DisplayPos.y * scale[1]};

  float *scaleConstant = new float[2];
  memcpy(scaleConstant, scale.data(), sizeof(float) * scale.size());
  commandList.pushConstants(pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                            sizeof(float) * scale.size(), scaleConstant);

  float *translateConstant = new float[2];
  memcpy(translateConstant, translate.data(), sizeof(float) * translate.size());
  commandList.pushConstants(
      pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * scale.size(),
      sizeof(float) * translate.size(), translateConstant);
}

void ImguiRenderer::loadFonts() {
  ImGuiIO &io = ImGui::GetIO();
  unsigned char *pixels = nullptr;
  TextureData textureData{};
  int width = 0, height = 0;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
  if (width > 0 || height > 0) {
    textureData.channels = 4;
    textureData.data = pixels;
    textureData.width = width;
    textureData.height = height;
    textureData.format = VK_FORMAT_R8G8B8A8_UNORM;
    fontTexture = resourceAllocator->createTexture2D(textureData);
  }

  io.Fonts->SetTexID(fontTexture.get());

  LOG_DEBUG("[ImGui] Fonts loaded");
}

} // namespace liquid
