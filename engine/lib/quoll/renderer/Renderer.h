#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/rhi/RenderDevice.h"
#include "RenderGraph.h"
#include "RendererDebugPanel.h"
#include "RendererOptions.h"

namespace quoll {

class Window;

struct DefaultGraphResources {
  rhi::TextureHandle sceneColor;

  rhi::TextureHandle depthBuffer;

  rhi::TextureHandle imguiColor;

  glm::vec4 defaultColor;
};

struct RendererTextures {
  RenderGraphResource<rhi::TextureHandle> finalTexture;

  RenderGraphResource<rhi::TextureHandle> sceneTexture;
};

class Renderer : NoCopyMove {
public:
  using GraphBuilderFn =
      std::function<RendererTextures(RenderGraph &, const RendererOptions &)>;

public:
  Renderer(RenderStorage &storage, const RendererOptions &options);

  ~Renderer() = default;

  void setGraphBuilder(GraphBuilderFn &&builderFn);

  void setFramebufferSize(glm::uvec2 size);

  void rebuildIfSettingsChanged();

  void execute(rhi::RenderCommandList &commandList, u32 frameIndex);

  constexpr rhi::TextureHandle getFinalTexture() const { return mFinalTexture; }

  constexpr rhi::TextureHandle getSceneTexture() const { return mSceneTexture; }

  constexpr debug::DebugPanel *getDebugPanel() { return &mDebugPanel; }

private:
  RenderStorage &mRenderStorage;

  bool mOptionsChanged = true;
  RendererOptions mOptions{};

  RenderGraph mGraph;
  GraphBuilderFn mBuilderFn{};

  rhi::TextureHandle mFinalTexture{0};
  rhi::TextureHandle mSceneTexture{0};

  debug::RendererDebugPanel mDebugPanel;
};

} // namespace quoll
