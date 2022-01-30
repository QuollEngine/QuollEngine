#include "liquid/core/Base.h"

#include "RenderGraph.h"
#include "RenderGraphAttachmentDescriptor.h"
#include "RenderGraphPipelineDescriptor.h"
#include "RenderGraphBuilder.h"

namespace liquid {

RenderGraphBuilder::RenderGraphBuilder(RenderGraph &graph_,
                                       RenderGraphPassBase *pass_)
    : graph(graph_), pass(pass_) {}

GraphResourceId RenderGraphBuilder::write(const String &name) {
  LIQUID_ASSERT(graph.hasResourceId(name),
                "Resource named \"" + name + "\" does not exist");
  GraphResourceId resourceId = graph.getResourceId(name);
  pass->addOutput(resourceId, {});

  if (graph.isSwapchain(resourceId) ||
      graph.getTextures().at(resourceId).sizeMethod ==
          AttachmentSizeMethod::SwapchainRelative) {
    pass->setSwapchainRelative(true);
  }
  return resourceId;
}

GraphResourceId RenderGraphBuilder::read(const String &name) {
  LIQUID_ASSERT(graph.hasResourceId(name),
                "Resource named \"" + name + "\" does not exist");
  GraphResourceId resourceId = graph.getResourceId(name);
  pass->addInput(resourceId);
  return resourceId;
}

GraphResourceId
RenderGraphBuilder::create(const PipelineDescriptor &descriptor) {
  auto resourceId = graph.addPipeline(descriptor);
  pass->addResource(resourceId);
  return resourceId;
}

} // namespace liquid
