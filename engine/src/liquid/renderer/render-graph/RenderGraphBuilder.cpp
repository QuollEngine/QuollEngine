#include "liquid/core/Base.h"

#include "RenderGraph.h"
#include "RenderGraphAttachmentDescription.h"
#include "RenderGraphPipelineDescription.h"
#include "RenderGraphBuilder.h"

namespace liquid {

RenderGraphBuilder::RenderGraphBuilder(RenderGraph &graph,
                                       RenderGraphPassBase *pass)
    : mGraph(graph), mPass(pass) {}

rhi::TextureHandle RenderGraphBuilder::write(const String &name) {
  LIQUID_ASSERT(mGraph.hasResourceId(name),
                "Resource named \"" + name + "\" does not exist");
  auto resourceId = mGraph.getResourceId(name);
  mPass->addOutput(resourceId, {});

  return resourceId;
}

rhi::TextureHandle RenderGraphBuilder::read(const String &name) {
  LIQUID_ASSERT(mGraph.hasResourceId(name),
                "Resource named \"" + name + "\" does not exist");
  auto resourceId = mGraph.getResourceId(name);
  mPass->addInput(resourceId);
  return resourceId;
}

GraphResourceId
RenderGraphBuilder::create(const RenderGraphPipelineDescription &descriptor) {
  auto resourceId = mGraph.addPipeline(descriptor);
  mPass->addResource(resourceId);
  return resourceId;
}

} // namespace liquid
