#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "liquid/renderer/SceneRenderer.h"

#include "liquid/renderer/RenderGraph.h"

namespace liquid {

Renderer::Renderer(Window &window, rhi::RenderDevice *device)
    : mDevice(device), mRenderStorage(mDevice) {}

void Renderer::render(RenderGraph &graph, rhi::RenderCommandList &commandList,
                      uint32_t frameIndex) {
  graph.build(mRenderStorage);
  graph.execute(commandList, frameIndex);
}

} // namespace liquid
