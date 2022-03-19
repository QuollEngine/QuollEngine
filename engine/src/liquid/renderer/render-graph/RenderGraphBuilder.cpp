#include "liquid/core/Base.h"

#include "RenderGraph.h"
#include "RenderGraphAttachmentDescription.h"
#include "RenderGraphPipelineDescription.h"
#include "RenderGraphBuilder.h"

namespace liquid {

RenderGraphBuilder::RenderGraphBuilder(RenderGraph &graph_,
                                       RenderGraphPassBase *pass_)
    : graph(graph_), pass(pass_) {}

rhi::TextureHandle RenderGraphBuilder::write(const String &name) {
  LIQUID_ASSERT(graph.hasResourceId(name),
                "Resource named \"" + name + "\" does not exist");
  auto resourceId = graph.getResourceId(name);
  pass->addOutput(resourceId, {});

  return resourceId;
}

rhi::TextureHandle RenderGraphBuilder::read(const String &name) {
  LIQUID_ASSERT(graph.hasResourceId(name),
                "Resource named \"" + name + "\" does not exist");
  auto resourceId = graph.getResourceId(name);
  pass->addInput(resourceId);
  return resourceId;
}

GraphResourceId
RenderGraphBuilder::create(const RenderGraphPipelineDescription &descriptor) {
  auto resourceId = graph.addPipeline(descriptor);
  pass->addResource(resourceId);
  return resourceId;
}

} // namespace liquid
