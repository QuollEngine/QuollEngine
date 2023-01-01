#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/PipelineDescription.h"

namespace liquid {

enum class VirtualPipelineHandle : size_t {};

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
   * @brief Get real pipeline
   *
   * @param handle Virtual pipeline handle
   * @return Pipeline
   */
  inline rhi::PipelineHandle get(VirtualPipelineHandle handle) const {
    return mRealResources.at(static_cast<size_t>(handle));
  }

  /**
   * @brief Set pipeline
   *
   * @param description Pipeline description
   * @return Virtual pipeline handle
   */
  VirtualPipelineHandle set(const rhi::PipelineDescription &description);

private:
  std::vector<rhi::PipelineDescription> mDescriptions;
  std::vector<rhi::PipelineHandle> mRealResources;
};

} // namespace liquid
