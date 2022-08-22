#include "liquid/core/Base.h"
#include "ResourceRegistry.h"

namespace liquid::rhi {

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
