#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "quoll/renderer/SceneRenderer.h"

#include "quoll/renderer/RenderGraph.h"

namespace quoll {

Renderer::Renderer(RenderStorage &storage, const RendererOptions &options)
    : mRenderStorage(storage), mGraph("Main"), mOptions(options) {}

void Renderer::setGraphBuilder(GraphBuilderFn &&builderFn) {
  mBuilderFn = builderFn;
}

void Renderer::setFramebufferSize(glm::uvec2 size) {
  mOptions.size = size;
  mOptionsChanged = true;
}

void Renderer::rebuildIfSettingsChanged() {
  if (!mOptionsChanged) {
    return;
  }

  mGraph.destroy(mRenderStorage);
  mGraph = RenderGraph("Main");
  auto res = mBuilderFn(mGraph, mOptions);
  mGraph.build(mRenderStorage);
  mSceneTexture = res.sceneTexture;
  mFinalTexture = res.finalTexture;
  mOptionsChanged = false;
}

void Renderer::execute(rhi::RenderCommandList &commandList,
                       uint32_t frameIndex) {

  mGraph.execute(commandList, frameIndex);
}

} // namespace quoll
