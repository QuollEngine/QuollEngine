#include "liquid/core/Base.h"
#include "MockPipeline.h"

namespace liquid::rhi {

MockPipeline::MockPipeline(const GraphicsPipelineDescription &description)
    : mType(Type::Graphics), mGraphicsDescription(description) {}

MockPipeline::MockPipeline(const ComputePipelineDescription &description)
    : mType(Type::Compute), mComputeDescription(description) {}

} // namespace liquid::rhi
