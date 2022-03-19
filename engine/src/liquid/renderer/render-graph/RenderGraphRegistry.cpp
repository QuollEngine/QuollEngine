#include "liquid/core/Base.h"
#include "RenderGraphRegistry.h"

namespace liquid {

void RenderGraphRegistry::addPipeline(GraphResourceId resourceId,
                                      rhi::PipelineHandle pipeline) {
  mPipelines.insert_or_assign(resourceId, pipeline);
}

void RenderGraphRegistry::addRenderPass(GraphResourceId resourceId,
                                        RenderGraphPassResult &&renderPass) {
  mRenderPasses.insert_or_assign(resourceId, std::move(renderPass));
}

} // namespace liquid
