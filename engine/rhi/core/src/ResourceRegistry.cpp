#include "liquid/core/Base.h"
#include "ResourceRegistry.h"

namespace liquid::rhi {

PipelineHandle
ResourceRegistry::setPipeline(const PipelineDescription &description,
                              PipelineHandle handle) {
  return mPipelines.setDescription(description, handle);
}

void ResourceRegistry::deletePipeline(PipelineHandle handle) {
  mPipelines.deleteDescription(handle);
}

} // namespace liquid::rhi
