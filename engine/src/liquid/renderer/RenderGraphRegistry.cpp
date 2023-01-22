#include "liquid/core/Base.h"
#include "RenderGraphRegistry.h"

namespace liquid {

VirtualPipelineHandle
RenderGraphRegistry::set(const rhi::GraphicsPipelineDescription &description) {
  mGraphicsPipelineDescriptions.push_back(description);
  mRealGraphicsPipelines.push_back(rhi::PipelineHandle::Invalid);

  return static_cast<VirtualPipelineHandle>(
      mGraphicsPipelineDescriptions.size() - 1);
}

VirtualComputePipelineHandle
RenderGraphRegistry::set(const rhi::ComputePipelineDescription &description) {
  mComputePipelineDescriptions.push_back(description);
  mRealComputePipelines.push_back(rhi::PipelineHandle::Invalid);

  return static_cast<VirtualComputePipelineHandle>(
      mComputePipelineDescriptions.size() - 1);
}

} // namespace liquid
