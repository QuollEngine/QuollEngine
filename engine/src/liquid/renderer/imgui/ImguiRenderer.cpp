#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

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

ImguiRenderer::ImguiRenderer(Window &window, rhi::ResourceRegistry &registry_)
    : registry(registry_) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window.getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "ImguiCustomBackend";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

  frameData.resize(1);

  loadFonts();

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");
}

ImguiRenderer::~ImguiRenderer() {
  registry.deleteTexture(fontTexture);

  for (auto &x : frameData) {
    registry.deleteBuffer(x.vertexBuffer);
    registry.deleteBuffer(x.indexBuffer);

    if (x.vertexBufferData)
      delete[] x.vertexBufferData;

    if (x.indexBufferData)
      delete[] x.indexBufferData;
  }

  frameData.clear();

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() { ImGui::Render(); }

void ImguiRenderer::draw(rhi::RenderCommandList &commandList,
                         rhi::PipelineHandle pipeline) {
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
    size_t vertexSize =
        getAlignedBufferSize(data->TotalVtxCount * sizeof(ImDrawVert));
    size_t indexSize =
        getAlignedBufferSize(data->TotalIdxCount * sizeof(ImDrawIdx));

    if (frameObj.vertexBufferSize < vertexSize) {
      if (frameObj.vertexBufferData) {
        delete[] frameObj.vertexBufferData;
      }
      frameObj.vertexBufferData = new char[vertexSize];
      frameObj.vertexBufferSize = vertexSize;
    }

    if (frameObj.indexBufferSize < indexSize) {
      if (frameObj.indexBufferData) {
        delete[] frameObj.indexBufferData;
      }
      frameObj.indexBufferData = new char[indexSize];
      frameObj.indexBufferSize = indexSize;
    }

    auto *vbDst = static_cast<ImDrawVert *>(frameObj.vertexBufferData);
    auto *ibDst = static_cast<ImDrawIdx *>(frameObj.indexBufferData);

    for (int n = 0; n < data->CmdListsCount; n++) {
      const ImDrawList *cmd_list = data->CmdLists[n];
      memcpy(vbDst, cmd_list->VtxBuffer.Data,
             cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
      memcpy(ibDst, cmd_list->IdxBuffer.Data,
             cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
      vbDst += cmd_list->VtxBuffer.Size;
      ibDst += cmd_list->IdxBuffer.Size;
    }

    if (registry.getBufferMap().hasDescription(frameObj.vertexBuffer)) {
      registry.updateBuffer(frameObj.vertexBuffer,
                            {rhi::BufferType::Vertex, frameObj.vertexBufferSize,
                             frameObj.vertexBufferData});
    } else {
      frameObj.vertexBuffer = registry.addBuffer({rhi::BufferType::Vertex,
                                                  frameObj.vertexBufferSize,
                                                  frameObj.vertexBufferData});
    }

    if (registry.getBufferMap().hasDescription(frameObj.indexBuffer)) {
      registry.updateBuffer(frameObj.indexBuffer,
                            {rhi::BufferType::Index, frameObj.indexBufferSize,
                             frameObj.indexBufferData});
    } else {
      frameObj.indexBuffer =
          registry.addBuffer({rhi::BufferType::Index, frameObj.indexBufferSize,
                              frameObj.indexBufferData});
    }
  }

  // @temporary
  // This workaround is necessary because
  // the buffer creation, deletion, and update
  // are deferred to the beginning of the next
  // frame. This still causes warnings but
  // the warnings will be resolved once textures
  // also use the new resource manager and this
  // function will be split into two parts
  // preparing the data and recording commands
  if (frameObj.firstTime) {
    frameObj.firstTime = false;
    return;
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

          auto handle = static_cast<rhi::TextureHandle>(
              reinterpret_cast<uintptr_t>(cmd->TextureId));

          rhi::Descriptor descriptor;
          descriptor.bind(0, std::vector<rhi::TextureHandle>{handle},
                          rhi::DescriptorType::CombinedImageSampler);

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
                                      rhi::RenderCommandList &commandList,
                                      int fbWidth, int fbHeight,
                                      rhi::PipelineHandle pipeline) {
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
  int width = 0, height = 0;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  rhi::TextureDescription description;
  if (width > 0 || height > 0) {
    description.size = width * height * 4;
    description.width = width;
    description.height = height;
    description.format = VK_FORMAT_R8G8B8A8_SRGB;
    description.usageFlags =
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    description.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    description.data = pixels;

    fontTexture = registry.addTexture(description);
  }

  io.Fonts->SetTexID(
      reinterpret_cast<void *>(static_cast<uintptr_t>(fontTexture)));

  LOG_DEBUG("[ImGui] Fonts loaded");
}

} // namespace liquid
