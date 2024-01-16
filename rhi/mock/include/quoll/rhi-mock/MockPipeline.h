#pragma once

#include "quoll/rhi/PipelineDescription.h"

namespace quoll::rhi {

class MockPipeline {
public:
  enum class Type { Graphics, Compute };

public:
  MockPipeline(const GraphicsPipelineDescription &description);

  MockPipeline(const ComputePipelineDescription &description);

  inline Type getType() const { return mType; }

  inline GraphicsPipelineDescription getGraphicsDescription() const {
    return mGraphicsDescription;
  }

  inline ComputePipelineDescription getComputeDecription() const {
    return mComputeDescription;
  }

private:
  GraphicsPipelineDescription mGraphicsDescription;
  ComputePipelineDescription mComputeDescription;

  Type mType;
};

} // namespace quoll::rhi
