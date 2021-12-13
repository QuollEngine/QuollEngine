#include "core/Base.h"
#include "core/EngineGlobals.h"

#include "renderer/Pipeline.h"
#include "ImguiRenderer.h"
#include <imgui_impl_glfw.h>
#include "renderer/vulkan/VulkanPipeline.h"

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
                             ResourceAllocator *resourceAllocator_)
    : vulkanContext(vulkanContext_), resourceAllocator(resourceAllocator_) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window->getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "ImguiCustomBackend";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

  frameData.resize(2);

  descriptorManager = new VulkanDescriptorManager(vulkanContext.getDevice());
  loadFonts();

  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");
}

ImguiRenderer::~ImguiRenderer() {
  delete descriptorManager;

  fontTexture = nullptr;
  frameData.clear();

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() { ImGui::Render(); }

void ImguiRenderer::draw(RenderCommandList &commandList,
                         const SharedPtr<Pipeline> &pipeline) {
  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  const auto &vulkanPipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

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

  setupRenderStates(data, commandList, fbWidth, fbHeight, vulkanPipeline);

  uint32_t indexOffset = 0;
  uint32_t vertexOffset = 0;
  for (int cmdListIdx = 0; cmdListIdx < data->CmdListsCount; ++cmdListIdx) {
    const ImDrawList *cmdList = data->CmdLists[cmdListIdx];
    for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; ++cmdIdx) {
      const ImDrawCmd *cmd = &cmdList->CmdBuffer[cmdIdx];
      if (cmd->UserCallback != NULL) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (cmd->UserCallback == ImDrawCallback_ResetRenderState) {
          setupRenderStates(data, commandList, fbWidth, fbHeight,
                            vulkanPipeline);
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
                {texture, createDescriptorFromTexture(texture, pipeline)});
          }

          commandList.bindDescriptorSets(vulkanPipeline->getPipelineLayout(),
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

VkDescriptorSet ImguiRenderer::createDescriptorFromTexture(
    Texture *texture, const SharedPtr<Pipeline> &pipeline) {
  const auto &vulkanPipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

  const auto &binder = std::dynamic_pointer_cast<VulkanTextureBinder>(
      texture->getResourceBinder());

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

  std::array<VkDescriptorSetLayout, 1> layouts{
      vulkanPipeline->getDescriptorLayout(0)};

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorManager->getDescriptorPool();
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts.data();
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

void ImguiRenderer::setupRenderStates(
    ImDrawData *data, RenderCommandList &commandList, int fbWidth, int fbHeight,
    const SharedPtr<VulkanPipeline> &pipeline) {
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
  commandList.pushConstants(pipeline->getPipelineLayout(),
                            VK_SHADER_STAGE_VERTEX_BIT, 0,
                            sizeof(float) * scale.size(), scaleConstant);

  float *translateConstant = new float[2];
  memcpy(translateConstant, translate.data(), sizeof(float) * translate.size());
  commandList.pushConstants(
      pipeline->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT,
      sizeof(float) * scale.size(), sizeof(float) * translate.size(),
      translateConstant);
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
