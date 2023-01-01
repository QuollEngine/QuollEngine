#include "liquid/core/Base.h"
#include "RenderGraphRegistry.h"

namespace liquid {

VirtualPipelineHandle
RenderGraphRegistry::set(const rhi::PipelineDescription &description) {
  mDescriptions.push_back(description);
  mRealResources.push_back(rhi::PipelineHandle::Invalid);

  return static_cast<VirtualPipelineHandle>(mDescriptions.size() - 1);
}

} // namespace liquid
