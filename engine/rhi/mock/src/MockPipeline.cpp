#include "liquid/core/Base.h"
#include "MockPipeline.h"

namespace quoll::rhi {

MockPipeline::MockPipeline(const GraphicsPipelineDescription &description)
    : mType(Type::Graphics), mGraphicsDescription(description) {}

MockPipeline::MockPipeline(const ComputePipelineDescription &description)
    : mType(Type::Compute), mComputeDescription(description) {}

} // namespace quoll::rhi
