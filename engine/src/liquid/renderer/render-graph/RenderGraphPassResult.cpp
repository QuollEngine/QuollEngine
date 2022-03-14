#include "liquid/core/Base.h"
#include "RenderGraphPassResult.h"

#include "liquid/core/EngineGlobals.h"

namespace liquid {

RenderGraphPassResult::RenderGraphPassResult(
    rhi::RenderPassHandle renderPass_,
    const std::vector<rhi::FramebufferHandle> &framebuffers_,
    const std::vector<VkClearValue> &clearValues_, uint32_t width_,
    uint32_t height_, uint32_t layers_)
    : renderPass(renderPass_), framebuffers(framebuffers_),
      clearValues(clearValues_), extent{width_, height_}, layers(layers_) {}

} // namespace liquid
