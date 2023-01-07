#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "liquid/renderer/SceneRenderer.h"

#include "liquid/renderer/RenderGraph.h"

namespace liquid {

Renderer::Renderer(AssetRegistry &assetRegistry, Window &window,
                   rhi::RenderDevice *device)
    : mGraphEvaluator(device), mDevice(device),
      mImguiRenderer(window, mShaderLibrary, mRenderStorage, device),
      mRenderStorage(device), mAssetRegistry(assetRegistry),
      mSceneRenderer(mShaderLibrary, mAssetRegistry, mRenderStorage, device) {}

void Renderer::render(RenderGraph &graph, rhi::RenderCommandList &commandList,
                      uint32_t frameIndex) {
  graph.compile(mDevice);

  mGraphEvaluator.build(graph);

  mGraphEvaluator.execute(commandList, graph, frameIndex);
}

} // namespace liquid
