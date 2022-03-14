#include "liquid/core/Base.h"
#include "RenderGraphRegistry.h"

namespace liquid {

void RenderGraphRegistry::addTexture(GraphResourceId resourceId,
                                     rhi::TextureHandle texture) {
  if (textures.find(resourceId) != textures.end()) {
    textures.at(resourceId) = texture;
  } else {
    textures.insert({resourceId, texture});
  }
}

void RenderGraphRegistry::addPipeline(GraphResourceId resourceId,
                                      rhi::PipelineHandle pipeline) {
  if (pipelines.find(resourceId) != pipelines.end()) {
    pipelines.at(resourceId) = pipeline;
  } else {
    pipelines.insert({resourceId, pipeline});
  }
}

void RenderGraphRegistry::addRenderPass(GraphResourceId resourceId,
                                        RenderGraphPassResult &&renderPass) {
  if (renderPasses.find(resourceId) != renderPasses.end()) {
    renderPasses.at(resourceId) = std::move(renderPass);
  } else {
    renderPasses.insert({resourceId, std::move(renderPass)});
  }
}

} // namespace liquid
