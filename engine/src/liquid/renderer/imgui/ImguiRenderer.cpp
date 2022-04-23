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

ImguiRenderer::ImguiRenderer(Window &window, rhi::ResourceRegistry &registry)
    : mRegistry(registry) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window.getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "ImguiCustomBackend";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

  static constexpr size_t FRAMES_IN_FLIGHT = 3;
  mFrameData.resize(FRAMES_IN_FLIGHT);

  loadFonts();

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");
}

ImguiRenderer::~ImguiRenderer() {
  mRegistry.deleteTexture(mFontTexture);

  for (auto &x : mFrameData) {
    mRegistry.deleteBuffer(x.vertexBuffer);
    mRegistry.deleteBuffer(x.indexBuffer);

    if (x.vertexBufferData)
      delete[] x.vertexBufferData;

    if (x.indexBufferData)
      delete[] x.indexBufferData;
  }

  mFrameData.clear();

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() {
  ImGui::Render();

  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);
  if (fbWidth <= 0 || fbHeight <= 0)
    return;

  mCurrentFrame = (mCurrentFrame + 1) % mFrameData.size();
  auto &frameObj = mFrameData.at(mCurrentFrame);

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

    frameObj.vertexBuffer =
        mRegistry.setBuffer({rhi::BufferType::Vertex, frameObj.vertexBufferSize,
                             frameObj.vertexBufferData},
                            frameObj.vertexBuffer);

    frameObj.indexBuffer =
        mRegistry.setBuffer({rhi::BufferType::Index, frameObj.indexBufferSize,
                             frameObj.indexBufferData},
                            frameObj.indexBuffer);
  }
}

void ImguiRenderer::draw(rhi::RenderCommandList &commandList,
                         rhi::PipelineHandle pipeline) {

  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  auto &frameObj = mFrameData.at(mCurrentFrame);
  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);
  if (fbWidth <= 0 || fbHeight <= 0)
    return;

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
    commandList.bindVertexBuffer(mFrameData.at(mCurrentFrame).vertexBuffer);
    commandList.bindIndexBuffer(mFrameData.at(mCurrentFrame).indexBuffer,
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

  commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                            scaleDataSize, scale.data());
  commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, scaleDataSize,
                            translateDataSize, translate.data());
}

void ImguiRenderer::loadFonts() {
  ImGuiIO &io = ImGui::GetIO();
  unsigned char *pixels = nullptr;
  int width = 0, height = 0;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  rhi::TextureDescription description;
  if (width > 0 || height > 0) {
    description.usage = rhi::TextureUsage::Color |
                        rhi::TextureUsage::TransferDestination |
                        rhi::TextureUsage::Sampled;
    description.size = width * height * 4;
    description.width = width;
    description.height = height;
    description.format = VK_FORMAT_R8G8B8A8_SRGB;
    description.data = pixels;

    mFontTexture = mRegistry.setTexture(description);
  }

  io.Fonts->SetTexID(
      reinterpret_cast<void *>(static_cast<uintptr_t>(mFontTexture)));

  LOG_DEBUG("[ImGui] Fonts loaded");
}

} // namespace liquid
