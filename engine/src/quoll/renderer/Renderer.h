#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/rhi/RenderDevice.h"
#include "RenderGraph.h"
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

class Renderer {
public:
  using GraphBuilderFn =
      std::function<RendererTextures(RenderGraph &, const RendererOptions &)>;

public:
  Renderer(RenderStorage &storage, const RendererOptions &options);

  ~Renderer() = default;
  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  void setGraphBuilder(GraphBuilderFn &&builderFn);

  void setFramebufferSize(glm::uvec2 size);

  void rebuildIfSettingsChanged();

  void execute(rhi::RenderCommandList &commandList, u32 frameIndex);

  inline rhi::TextureHandle getFinalTexture() const { return mFinalTexture; }

  inline rhi::TextureHandle getSceneTexture() const { return mSceneTexture; }

private:
  RenderStorage &mRenderStorage;

  bool mOptionsChanged = true;
  RendererOptions mOptions{};

  RenderGraph mGraph;
  GraphBuilderFn mBuilderFn{};

  rhi::TextureHandle mFinalTexture{0};
  rhi::TextureHandle mSceneTexture{0};
};

} // namespace quoll
