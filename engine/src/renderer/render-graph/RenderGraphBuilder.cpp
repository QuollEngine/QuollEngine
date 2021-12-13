#include "core/Base.h"

#include "RenderGraph.h"
#include "RenderGraphAttachmentDescriptor.h"
#include "RenderGraphPipelineDescriptor.h"
#include "RenderGraphBuilder.h"

namespace liquid {

RenderGraphBuilder::RenderGraphBuilder(RenderGraph &graph_,
                                       RenderGraphPassInterface *pass_)
    : graph(graph_), pass(pass_) {}

GraphResourceId
RenderGraphBuilder::write(const String &name,
                          const RenderPassAttachment &attachment) {
  GraphResourceId resourceId = graph.addAttachment(name, attachment);
  pass->addOutput(resourceId);
  return resourceId;
}

GraphResourceId RenderGraphBuilder::writeSwapchain(
    const String &name, const RenderPassSwapchainAttachment &attachment) {
  GraphResourceId resourceId = graph.addSwapchainAttachment(name, attachment);
  pass->addOutput(resourceId);
  pass->setSwapchainRelative(true);
  return resourceId;
}

GraphResourceId RenderGraphBuilder::read(const String &name) {
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
