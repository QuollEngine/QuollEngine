#include "liquid/core/Base.h"
#include "ResourceRegistry.h"

namespace liquid::rhi {

RenderPassHandle
ResourceRegistry::setRenderPass(const RenderPassDescription &description,
                                RenderPassHandle handle) {
  return mRenderPasses.setDescription(description, handle);
}

void ResourceRegistry::deleteRenderPass(rhi::RenderPassHandle handle) {
  mRenderPasses.deleteDescription(handle);
}

FramebufferHandle
ResourceRegistry::setFramebuffer(const FramebufferDescription &description,
                                 FramebufferHandle handle) {
  return mFramebuffers.setDescription(description, handle);
}

void ResourceRegistry::deleteFramebuffer(FramebufferHandle handle) {
  mFramebuffers.deleteDescription(handle);
}

PipelineHandle
ResourceRegistry::setPipeline(const PipelineDescription &description,
                              PipelineHandle handle) {
  return mPipelines.setDescription(description, handle);
}

void ResourceRegistry::deletePipeline(PipelineHandle handle) {
  mPipelines.deleteDescription(handle);
}

} // namespace liquid::rhi
