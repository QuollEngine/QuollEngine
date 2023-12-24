#pragma once

#include "quoll/rhi/PipelineDescription.h"

namespace quoll::rhi {

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
   * @brief Create compute pipeline
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

  /**
   * @brief Get graphics pipeline description
   *
   * @return Graphics pipeline description
   */
  inline GraphicsPipelineDescription getGraphicsDescription() const {
    return mGraphicsDescription;
  }

  /**
   * @brief Get compute pipeline description
   *
   * @return Compute pipeline description
   */
  inline ComputePipelineDescription getComputeDecription() const {
    return mComputeDescription;
  }

private:
  GraphicsPipelineDescription mGraphicsDescription;
  ComputePipelineDescription mComputeDescription;

  Type mType;
};

} // namespace quoll::rhi
