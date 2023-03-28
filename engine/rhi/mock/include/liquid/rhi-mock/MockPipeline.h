#pragma once

#include "liquid/rhi/PipelineDescription.h"

namespace liquid::rhi {

/**
 * @brief Mock pipeline
 */
class MockPipeline {
public:
  /**
   * @brief Pipeline type
   */
  enum class Type { Graphics, Compute };

public:
  /**
   * @brief Create graphics pipeline
   *
   * @param description Graphics pipeline description
   */
  MockPipeline(const GraphicsPipelineDescription &description);

  /**
   * @brief Create comppute pipeline
   *
   * @param description Compute pipeline description
   */
  MockPipeline(const ComputePipelineDescription &description);

  /**
   * @brief Get pipeline type
   *
   * @return Pipeline type
   */
  inline Type getType() const { return mType; }

private:
  GraphicsPipelineDescription mGraphicsDescription;
  ComputePipelineDescription mComputeDescription;

  Type mType;
};

} // namespace liquid::rhi
