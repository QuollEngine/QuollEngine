#include "liquid/core/Base.h"
#include "RenderGraphPassBase.h"

namespace liquid {
RenderGraphPassBase::RenderGraphPassBase(const String &name,
                                         GraphResourceId renderPass)
    : mName(name), mRenderPass(renderPass) {}

void RenderGraphPassBase::build(RenderGraphBuilder &&builder) {
  if (mDirty) {
    buildInternal(builder);
    mDirty = false;
  }
}

void RenderGraphPassBase::addInput(rhi::TextureHandle resourceId) {
  mInputs.push_back(resourceId);
}

void RenderGraphPassBase::addOutput(rhi::TextureHandle resourceId,
                                    const RenderPassAttachment &attachment) {
  mOutputs.insert({resourceId, attachment});
}

void RenderGraphPassBase::addResource(GraphResourceId resourceId) {
  mResources.push_back(resourceId);
}

} // namespace liquid
