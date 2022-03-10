#include "liquid/core/Base.h"
#include "RenderGraphRegistry.h"

namespace liquid {

void RenderGraphRegistry::addTexture(GraphResourceId resourceId,
                                     TextureHandle texture) {
  if (textures.find(resourceId) != textures.end()) {
    textures.at(resourceId) = texture;
  } else {
    textures.insert({resourceId, texture});
  }
}

void RenderGraphRegistry::addPipeline(GraphResourceId resourceId,
                                      const SharedPtr<Pipeline> &pipeline) {
  if (pipelines.find(resourceId) != pipelines.end()) {
    pipelines.at(resourceId) = pipeline;
  } else {
    pipelines.insert({resourceId, pipeline});
  }
}

void RenderGraphRegistry::addRenderPass(
    GraphResourceId resourceId, const SharedPtr<RenderPass> &renderPass) {
  if (renderPasses.find(resourceId) != renderPasses.end()) {
    renderPasses.at(resourceId) = renderPass;
  } else {
    renderPasses.insert({resourceId, renderPass});
  }
}

} // namespace liquid
