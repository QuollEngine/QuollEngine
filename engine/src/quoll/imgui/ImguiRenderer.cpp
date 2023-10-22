#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/renderer/TextureUtils.h"

#include "ImguiRenderer.h"

#include <imgui_impl_glfw.h>

static constexpr u64 BufferMemoryAlignment = 256;

/**
 * @brief Aligns given size based on specified alignment
 *
 * @param size Size to align
 * @return Aligned size
 */
static inline u64 getAlignedBufferSize(u64 size) {
  return ((size - 1) / BufferMemoryAlignment + 1) * BufferMemoryAlignment;
}

namespace quoll {

ImguiRenderer::ImguiRenderer(Window &window, RenderStorage &renderStorage)
    : mRenderStorage(renderStorage), mDevice(renderStorage.getDevice()) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window.getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "QuollRHI";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  io.IniFilename = nullptr;

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  Engine::getLogger().info() << "Imgui initialized";

  mRenderStorage.createShader("__engine.imgui.default.vertex",
                              {Engine::getShadersPath() / "imgui.vert.spv"});
  mRenderStorage.createShader("__engine.imgui.default.fragment",
                              {Engine::getShadersPath() / "imgui.frag.spv"});

  for (auto &x : mFrameData) {
    quoll::rhi::BufferDescription vertexDesc{};
    vertexDesc.usage = quoll::rhi::BufferUsage::Vertex;
    vertexDesc.size = 1;
    vertexDesc.mapped = true;
    vertexDesc.debugName = "imgui vertex";

    x.vertexBuffer = renderStorage.createBuffer(vertexDesc);

    quoll::rhi::BufferDescription indexDesc{};
    indexDesc.usage = quoll::rhi::BufferUsage::Index;
    indexDesc.size = 1;
    indexDesc.mapped = true;
    indexDesc.debugName = "imgui index";

    x.indexBuffer = renderStorage.createBuffer(indexDesc);
  }
}

ImguiRenderer::~ImguiRenderer() {
  for (auto &x : mFrameData) {
    mDevice->destroyBuffer(x.vertexBuffer.getHandle());
    mDevice->destroyBuffer(x.indexBuffer.getHandle());
  }
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

ImguiRenderPassData ImguiRenderer::attach(RenderGraph &graph,
                                          const RendererOptions &options) {
  QuollAssert(mReady, "Fonts are not built. Call ImguiRenderer::loadFonts "
                      "before starting rendering");

  rhi::TextureDescription imguiDesc{};
  imguiDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  imguiDesc.width = options.size.x;
  imguiDesc.height = options.size.y;
  imguiDesc.layerCount = 1;
  imguiDesc.format = rhi::Format::Rgba8Srgb;
  imguiDesc.debugName = "Imgui color";
  auto imgui = graph.create(imguiDesc);

  auto &pass = graph.addGraphicsPass("imgui");
  pass.write(imgui, AttachmentType::Color, mClearColor);

  auto pipeline = mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
      mRenderStorage.getShader("__engine.imgui.default.vertex"),
      mRenderStorage.getShader("__engine.imgui.default.fragment"),
      rhi::PipelineVertexInputLayout{
          {rhi::PipelineVertexInputBinding{0, sizeof(ImDrawVert),
                                           rhi::VertexInputRate::Vertex}},
          {rhi::PipelineVertexInputAttribute{0, 0, rhi::Format::Rg32Float,
                                             IM_OFFSETOF(ImDrawVert, pos)},
           rhi::PipelineVertexInputAttribute{1, 0, rhi::Format::Rg32Float,
                                             IM_OFFSETOF(ImDrawVert, uv)},
           rhi::PipelineVertexInputAttribute{2, 0, rhi::Format::Rgba8Unorm,
                                             IM_OFFSETOF(ImDrawVert, col)}}

      },
      rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
      rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                              rhi::FrontFace::CounterClockwise},
      rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
          true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::OneMinusSrcAlpha,
          rhi::BlendOp::Add, rhi::BlendFactor::One,
          rhi::BlendFactor::OneMinusSrcAlpha, rhi::BlendOp::Add}}},
      {},
      {},
      "imgui"});

  pass.addPipeline(pipeline);

  pass.setExecutor(
      [this, pipeline](rhi::RenderCommandList &commandList, u32 frameIndex) {
        draw(commandList, pipeline, frameIndex);
      });

  return {pass, imgui};
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() { ImGui::Render(); }

void ImguiRenderer::updateFrameData(u32 frameIndex) {
  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);
  if (fbWidth <= 0 || fbHeight <= 0)
    return;

  auto &frameObj = mFrameData.at(frameIndex);

  if (data->TotalVtxCount <= 0) {
    return;
  }

  usize vertexSize =
      getAlignedBufferSize(data->TotalVtxCount * sizeof(ImDrawVert));
  usize indexSize =
      getAlignedBufferSize(data->TotalIdxCount * sizeof(ImDrawIdx));

  if (frameObj.vertexBufferSize < vertexSize) {
    frameObj.vertexBuffer.resize(vertexSize);
    frameObj.vertexBufferSize = vertexSize;
  }

  if (frameObj.indexBufferSize < indexSize) {
    frameObj.indexBuffer.resize(indexSize);
    frameObj.indexBufferSize = indexSize;
  }

  auto *vbDst = static_cast<ImDrawVert *>(frameObj.vertexBuffer.map());
  auto *ibDst = static_cast<ImDrawIdx *>(frameObj.indexBuffer.map());

  for (int n = 0; n < data->CmdListsCount; n++) {
    const ImDrawList *cmd_list = data->CmdLists[n];
    memcpy(vbDst, cmd_list->VtxBuffer.Data,
           cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
    memcpy(ibDst, cmd_list->IdxBuffer.Data,
           cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
    vbDst += cmd_list->VtxBuffer.Size;
    ibDst += cmd_list->IdxBuffer.Size;
  }

  frameObj.vertexBuffer.unmap();
  frameObj.indexBuffer.unmap();
}

void ImguiRenderer::draw(rhi::RenderCommandList &commandList,
                         rhi::PipelineHandle pipeline, u32 frameIndex) {
  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  auto &frameObj = mFrameData.at(frameIndex);
  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);

  f32 realFbWidth = static_cast<f32>(fbWidth);
  f32 realFbHeight = static_cast<f32>(fbHeight);

  if (fbWidth <= 0 || fbHeight <= 0)
    return;

  commandList.bindDescriptor(pipeline, 0,
                             mRenderStorage.getGlobalTexturesDescriptor());

  setupRenderStates(data, commandList, fbWidth, fbHeight, pipeline, frameIndex);

  u32 indexOffset = 0;
  u32 vertexOffset = 0;
  for (int cmdListIdx = 0; cmdListIdx < data->CmdListsCount; ++cmdListIdx) {
    const ImDrawList *cmdList = data->CmdLists[cmdListIdx];
    for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; ++cmdIdx) {
      const ImDrawCmd *cmd = &cmdList->CmdBuffer[cmdIdx];
      if (cmd->UserCallback != NULL) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (cmd->UserCallback == ImDrawCallback_ResetRenderState) {
          setupRenderStates(data, commandList, fbWidth, fbHeight, pipeline,
                            frameIndex);
        } else {
          cmd->UserCallback(cmdList, cmd);
        }
      } else {
        glm::vec2 clipRectMin;
        glm::vec2 clipRectMax;

        clipRectMin.x =
            (cmd->ClipRect.x - data->DisplayPos.x) * data->FramebufferScale.x;
        clipRectMin.y =
            (cmd->ClipRect.y - data->DisplayPos.y) * data->FramebufferScale.y;
        clipRectMax.x =
            (cmd->ClipRect.z - data->DisplayPos.x) * data->FramebufferScale.x;
        clipRectMax.y =
            (cmd->ClipRect.w - data->DisplayPos.y) * data->FramebufferScale.y;

        if (clipRectMin.x < 0.0f) {
          clipRectMin.x = 0.0f;
        }
        if (clipRectMin.y < 0.0f) {
          clipRectMin.y = 0.0f;
        }

        if (clipRectMax.x > realFbWidth) {
          clipRectMax.x = realFbWidth;
        }
        if (clipRectMax.y > realFbHeight) {
          clipRectMax.y = realFbHeight;
        }

        if (clipRectMax.x <= clipRectMin.x || clipRectMax.y <= clipRectMin.y)
          continue;

        commandList.setScissor(clipRectMin, clipRectMax - clipRectMin);
        commandList.bindPipeline(pipeline);

        glm::uvec4 textureData{
            static_cast<u32>(reinterpret_cast<uptr>(cmd->TextureId)),
            rhi::castHandleToUint(mRenderStorage.getDefaultSampler()), 0, 0};

        commandList.pushConstants(pipeline, rhi::ShaderStage::Fragment,
                                  sizeof(glm::mat4), sizeof(glm::uvec4),
                                  glm::value_ptr(textureData));

        commandList.drawIndexed(
            cmd->ElemCount, cmd->IdxOffset + indexOffset,
            static_cast<i32>(cmd->VtxOffset + vertexOffset));
      }
    }
    indexOffset += cmdList->IdxBuffer.Size;
    vertexOffset += cmdList->VtxBuffer.Size;
  }
}

void ImguiRenderer::setupRenderStates(ImDrawData *data,
                                      rhi::RenderCommandList &commandList,
                                      int fbWidth, int fbHeight,
                                      rhi::PipelineHandle pipeline,
                                      u32 frameIndex) {
  if (data->TotalVtxCount > 0) {
    std::array<u64, 1> offsets{0};
    commandList.bindVertexBuffers(
        std::array{mFrameData.at(frameIndex).vertexBuffer.getHandle()},
        offsets);
    commandList.bindIndexBuffer(
        mFrameData.at(frameIndex).indexBuffer.getHandle(),
        sizeof(ImDrawIdx) == 2 ? rhi::IndexType::Uint16
                               : rhi::IndexType::Uint32);
  }

  commandList.setViewport({0, 0}, {fbWidth, fbHeight}, {0.0f, 1.0f});

  f32 L = data->DisplayPos.x;
  f32 R = data->DisplayPos.x + data->DisplaySize.x;
  f32 T = data->DisplayPos.y;
  f32 B = data->DisplayPos.y + data->DisplaySize.y;

  static constexpr usize MatrixSize = 16;
  const f32 SCALE_FACTOR = 2.0f;
  std::array<f32, MatrixSize> mvp{SCALE_FACTOR / (R - L),
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  SCALE_FACTOR / (T - B),
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  1.0f / SCALE_FACTOR,
                                  0.0f,
                                  (R + L) / (L - R),
                                  (T + B) / (B - T),
                                  1.0f / SCALE_FACTOR,
                                  1.0f

  };

  commandList.pushConstants(
      pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
      static_cast<u32>(sizeof(f32) * mvp.size()), mvp.data());
}

void ImguiRenderer::setClearColor(const glm::vec4 &clearColor) {
  mClearColor = clearColor;
}

void ImguiRenderer::buildFonts() {
  ImGuiIO &io = ImGui::GetIO();

  unsigned char *pixels = nullptr;
  int width = 0, height = 0;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  rhi::TextureDescription description;
  if (width > 0 || height > 0) {
    description.usage = rhi::TextureUsage::Color |
                        rhi::TextureUsage::TransferDestination |
                        rhi::TextureUsage::Sampled;
    description.width = width;
    description.height = height;
    description.format = rhi::Format::Rgba8Srgb;
    description.debugName = "Imgui font";
    mFontTexture = mRenderStorage.createTexture(description);

    TextureUtils::copyDataToTexture(
        mRenderStorage.getDevice(), pixels, mFontTexture,
        rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
        {TextureAssetLevel{0, static_cast<usize>(width) * height * 4,
                           static_cast<u32>(width), static_cast<u32>(height)}});
  }

  io.Fonts->SetTexID(reinterpret_cast<void *>(static_cast<uptr>(mFontTexture)));

  {
    auto &&stream = Engine::getLogger().info();
    stream << "Imgui fonts loaded: ";
    for (auto *font : io.Fonts->Fonts) {
      stream << "\"" << font->ConfigData->Name << "\"; ";
    }
  }

  mReady = true;
}

} // namespace quoll
