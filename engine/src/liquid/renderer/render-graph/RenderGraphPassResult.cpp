#include "liquid/core/Base.h"
#include "RenderGraphPassResult.h"

#include "liquid/core/EngineGlobals.h"

namespace liquid {

RenderGraphPassResult::RenderGraphPassResult(
    rhi::RenderPassHandle renderPass,
    const std::vector<rhi::FramebufferHandle> &framebuffers,
    const std::vector<VkClearValue> &clearValues, uint32_t width,
    uint32_t height, uint32_t layers)
    : mRenderPass(renderPass), mFramebuffers(framebuffers),
      mClearValues(clearValues), mExtent{width, height}, mLayers(layers) {}

} // namespace liquid
