#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/PipelineDescription.h"

namespace liquid {

enum class VirtualPipelineHandle : size_t {};

enum class VirtualComputePipelineHandle : size_t {};

/**
 * @brief Render graph registry
 *
 * Stores transient and virtual resources
 * that gets available after graph is evaluated
 */
class RenderGraphRegistry {
  friend class RenderGraphEvaluator;

public:
  /**
   * @brief Get real graphics pipeline
   *
   * @param handle Virtual graphics pipeline handle
   * @return Graphics pipeline
   */
  inline rhi::PipelineHandle get(VirtualPipelineHandle handle) const {
    return mRealGraphicsPipelines.at(static_cast<size_t>(handle));
  }

  /**
   * @brief Get real compute pipeline
   *
   * @param handle Virtual compute pipeline handle
   * @return Compute pipeline
   */
  inline rhi::PipelineHandle get(VirtualComputePipelineHandle handle) const {
    return mRealComputePipelines.at(static_cast<size_t>(handle));
  }

  /**
   * @brief Set graphics pipeline
   *
   * @param description Graphics pipeline description
   * @return Virtual pipeline handle
   */
  VirtualPipelineHandle
  set(const rhi::GraphicsPipelineDescription &description);

  /**
   * @brief Set compute pipeline
   *
   * @param description Compute pipeline description
   * @return Virtual pipeline handle
   */
  VirtualComputePipelineHandle
  set(const rhi::ComputePipelineDescription &description);

private:
  std::vector<rhi::GraphicsPipelineDescription> mGraphicsPipelineDescriptions;
  std::vector<rhi::ComputePipelineDescription> mComputePipelineDescriptions;

  std::vector<rhi::PipelineHandle> mRealGraphicsPipelines;
  std::vector<rhi::PipelineHandle> mRealComputePipelines;
};

} // namespace liquid
