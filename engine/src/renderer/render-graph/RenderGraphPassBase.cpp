#include "core/Base.h"
#include "RenderGraphPassBase.h"

namespace liquid {
RenderGraphPassBase::RenderGraphPassBase(const String &name_,
                                         GraphResourceId renderPass_)
    : name(name_), renderPass(renderPass_) {}

void RenderGraphPassBase::build(RenderGraphBuilder &&builder) {
  if (dirty) {
    buildInternal(builder);
    dirty = false;
  }
}

void RenderGraphPassBase::addInput(GraphResourceId resourceId) {
  inputs.push_back(resourceId);
}

void RenderGraphPassBase::addOutput(GraphResourceId resourceId) {
  outputs.push_back(resourceId);
}

void RenderGraphPassBase::addResource(GraphResourceId resourceId) {
  resources.push_back(resourceId);
}

void RenderGraphPassBase::setSwapchainRelative(bool swapchainRelative_) {
  swapchainRelative = swapchainRelative_;
}

} // namespace liquid
