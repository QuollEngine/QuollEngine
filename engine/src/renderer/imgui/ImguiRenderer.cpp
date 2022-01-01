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
                             ResourceAllocator *resourceAllocator_)
    : vulkanContext(vulkanContext_), resourceAllocator(resourceAllocator_) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window->getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "ImguiCustomBackend";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

  frameData.resize(2);

  loadFonts();

  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");
}

ImguiRenderer::~ImguiRenderer() {
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

  setupRenderStates(data, commandList, fbWidth, fbHeight, pipeline);

  uint32_t indexOffset = 0;
  uint32_t vertexOffset = 0;
  for (int cmdListIdx = 0; cmdListIdx < data->CmdListsCount; ++cmdListIdx) {
    const ImDrawList *cmdList = data->CmdLists[cmdListIdx];
    for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; ++cmdIdx) {
      const ImDrawCmd *cmd = &cmdList->CmdBuffer[cmdIdx];
      if (cmd->UserCallback != NULL) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (cmd->UserCallback == ImDrawCallback_ResetRenderState) {
          setupRenderStates(data, commandList, fbWidth, fbHeight, pipeline);
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
          commandList.bindPipeline(pipeline);

          const auto &texture =
              static_cast<Texture *>(cmd->TextureId)->shared_from_this();
          Descriptor descriptor;
          descriptor.bind(0, {texture}, DescriptorType::CombinedImageSampler);

          commandList.bindDescriptor(pipeline, 0, descriptor);
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

void ImguiRenderer::setupRenderStates(ImDrawData *data,
                                      RenderCommandList &commandList,
                                      int fbWidth, int fbHeight,
                                      const SharedPtr<Pipeline> &pipeline) {
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

  uint32_t scaleDataSize = static_cast<uint32_t>(sizeof(float) * scale.size());
  uint32_t translateDataSize =
      static_cast<uint32_t>(sizeof(float) * translate.size());

  float *scaleConstant = new float[2];

  memcpy(scaleConstant, scale.data(), scaleDataSize);
  commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                            scaleDataSize, scaleConstant);

  float *translateConstant = new float[2];
  memcpy(translateConstant, translate.data(), scaleDataSize);
  commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, scaleDataSize,
                            translateDataSize, translateConstant);
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
