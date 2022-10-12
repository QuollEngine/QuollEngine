#include "liquid/core/Base.h"
#include "RenderGraphRegistry.h"

namespace liquid::rhi {

VirtualPipelineHandle
RenderGraphRegistry::set(const PipelineDescription &description) {
  mDescriptions.push_back(description);
  mRealResources.push_back(PipelineHandle::Invalid);

  return static_cast<VirtualPipelineHandle>(mDescriptions.size() - 1);
}

} // namespace liquid::rhi
