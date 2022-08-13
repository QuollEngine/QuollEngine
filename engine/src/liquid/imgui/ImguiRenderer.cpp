#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/core/Engine.h"

#include "ImguiRenderer.h"

#include <imgui_impl_glfw.h>

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

ImguiRenderer::ImguiRenderer(Window &window, ShaderLibrary &shaderLibrary,
                             rhi::ResourceRegistry &registry,
                             rhi::RenderDevice *device)
    : mRegistry(registry), mShaderLibrary(shaderLibrary), mDevice(device) {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForVulkan(window.getInstance(), true);

  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "ImguiCustomBackend";
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  io.IniFilename = nullptr;

  static constexpr size_t FramesInFlight = 2;
  mFrameData.resize(FramesInFlight);

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");

  mShaderLibrary.addShader("__engine.imgui.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/imgui.vert.spv"}));
  mShaderLibrary.addShader("__engine.imgui.default.fragment",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/imgui.frag.spv"}));

  for (auto &x : mFrameData) {
    x.vertexBuffer =
        mDevice->createBuffer({liquid::rhi::BufferType::Vertex, 1});
    x.indexBuffer = mDevice->createBuffer({liquid::rhi::BufferType::Index, 1});
  }
}

ImguiRenderer::~ImguiRenderer() {
  mRegistry.deleteTexture(mFontTexture);

  mFrameData.clear();

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

ImguiRenderPassData ImguiRenderer::attach(rhi::RenderGraph &graph) {
  LIQUID_ASSERT(mReady, "Fonts are not built. Call ImguiRenderer::loadFonts "
                        "before starting rendering");

  static constexpr uint32_t FramebufferSizePercentage = 100;

  rhi::TextureDescription imguiDesc{};
  imguiDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  imguiDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  imguiDesc.width = FramebufferSizePercentage;
  imguiDesc.height = FramebufferSizePercentage;
  imguiDesc.layers = 1;
  imguiDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
  auto imgui = mRegistry.setTexture(imguiDesc);

  auto &pass = graph.addPass("imgui");
  pass.write(imgui, mClearColor);

  auto pipeline = mRegistry.setPipeline(rhi::PipelineDescription{
      mShaderLibrary.getShader("__engine.imgui.default.vertex"),
      mShaderLibrary.getShader("__engine.imgui.default.fragment"),
      rhi::PipelineVertexInputLayout{
          {rhi::PipelineVertexInputBinding{0, sizeof(ImDrawVert),
                                           rhi::VertexInputRate::Vertex}},
          {rhi::PipelineVertexInputAttribute{0, 0, VK_FORMAT_R32G32_SFLOAT,
                                             IM_OFFSETOF(ImDrawVert, pos)},
           rhi::PipelineVertexInputAttribute{1, 0, VK_FORMAT_R32G32_SFLOAT,
                                             IM_OFFSETOF(ImDrawVert, uv)},
           rhi::PipelineVertexInputAttribute{2, 0, VK_FORMAT_R8G8B8A8_UNORM,
                                             IM_OFFSETOF(ImDrawVert, col)}}

      },
      rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
      rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                              rhi::FrontFace::CounterClockwise},
      rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
          true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::OneMinusSrcAlpha,
          rhi::BlendOp::Add, rhi::BlendFactor::One,
          rhi::BlendFactor::OneMinusSrcAlpha, rhi::BlendOp::Add}}}});

  pass.addPipeline(pipeline);

  pass.setExecutor([this, pipeline](rhi::RenderCommandList &commandList) {
    draw(commandList, pipeline);
  });

  return {pass, imgui};
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() { ImGui::Render(); }

void ImguiRenderer::updateFrameData(uint32_t frameIndex) {
  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);
  if (fbWidth <= 0 || fbHeight <= 0)
    return;

  mCurrentFrame = frameIndex;
  auto &frameObj = mFrameData.at(mCurrentFrame);

  if (data->TotalVtxCount <= 0) {
    return;
  }

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

    frameObj.vertexBuffer.resize(vertexSize);
  }

  if (frameObj.indexBufferSize < indexSize) {
    if (frameObj.indexBufferData) {
      delete[] frameObj.indexBufferData;
    }
    frameObj.indexBufferData = new char[indexSize];
    frameObj.indexBufferSize = indexSize;

    frameObj.indexBuffer.resize(indexSize);
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

  frameObj.vertexBuffer.update(frameObj.vertexBufferData);
  frameObj.indexBuffer.update(frameObj.indexBufferData);
}

void ImguiRenderer::draw(rhi::RenderCommandList &commandList,
                         rhi::PipelineHandle pipeline) {
  auto *data = ImGui::GetDrawData();

  if (!data)
    return;

  auto &frameObj = mFrameData.at(mCurrentFrame);
  int fbWidth = (int)(data->DisplaySize.x * data->FramebufferScale.x);
  int fbHeight = (int)(data->DisplaySize.y * data->FramebufferScale.y);

  float realFbWidth = static_cast<float>(fbWidth);
  float realFbHeight = static_cast<float>(fbHeight);

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
    indexOffset += cmdList->IdxBuffer.Size;
    vertexOffset += cmdList->VtxBuffer.Size;
  }
}

void ImguiRenderer::setupRenderStates(ImDrawData *data,
                                      rhi::RenderCommandList &commandList,
                                      int fbWidth, int fbHeight,
                                      rhi::PipelineHandle pipeline) {
  if (data->TotalVtxCount > 0) {
    commandList.bindVertexBuffer(
        mFrameData.at(mCurrentFrame).vertexBuffer.getHandle());
    commandList.bindIndexBuffer(
        mFrameData.at(mCurrentFrame).indexBuffer.getHandle(),
        sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
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

void ImguiRenderer::useConfigPath(const String &path) {
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = path.c_str();
  ImGui::LoadIniSettingsFromDisk(io.IniFilename);
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

  mReady = true;
}

} // namespace liquid
