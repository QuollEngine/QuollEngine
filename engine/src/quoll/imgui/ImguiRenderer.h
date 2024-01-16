#pragma once

#include "quoll/imgui/Imgui.h"
#include "quoll/renderer/RenderGraph.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/renderer/RendererOptions.h"
#include "quoll/rhi/RenderCommandList.h"
#include "quoll/rhi/RenderDevice.h"
#include "quoll/window/Window.h"

namespace quoll {

struct ImguiRenderPassData {
  RenderGraphPass &pass;

  RenderGraphResource<rhi::TextureHandle> imguiColor;
};

class ImguiRenderer {
  struct FrameData {
    rhi::Buffer vertexBuffer;

    usize vertexBufferSize = 0;

    void *vertexBufferData = nullptr;

    rhi::Buffer indexBuffer;

    usize indexBufferSize = 0;

    void *indexBufferData = nullptr;
  };

  static constexpr const glm::vec4 DefaultClearColor{0.0f, 0.0f, 0.0f, 1.0f};

public:
  ImguiRenderer(Window &window, RenderStorage &renderStorage);

  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  ImguiRenderPassData attach(RenderGraph &graph,
                             const RendererOptions &options);

  void setClearColor(const glm::vec4 &clearColor);

  void buildFonts();

  void beginRendering();

  void endRendering();

  void updateFrameData(u32 frameIndex);

  void draw(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
            u32 frameIndex);

private:
  void setupRenderStates(ImDrawData *data, rhi::RenderCommandList &commandList,
                         int fbWidth, int fbHeight,
                         rhi::PipelineHandle pipeline, u32 frameIndex);

private:
  RenderStorage &mRenderStorage;
  rhi::TextureHandle mFontTexture = rhi::TextureHandle::Null;

  std::array<FrameData, rhi::RenderDevice::NumFrames> mFrameData;

  glm::vec4 mClearColor{DefaultClearColor};

  bool mReady = false;

  rhi::RenderDevice *mDevice;
};

} // namespace quoll
