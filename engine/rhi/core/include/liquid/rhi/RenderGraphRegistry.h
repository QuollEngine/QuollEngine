#pragma once

#include "RenderHandle.h"
#include "PipelineDescription.h"

namespace liquid::rhi {

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
  inline PipelineHandle get(VirtualPipelineHandle handle) const {
    return mRealResources.at(static_cast<size_t>(handle));
  }

  /**
   * @brief Set pipeline
   *
   * @param description Pipeline description
   * @return Virtual pipeline handle
   */
  VirtualPipelineHandle set(const PipelineDescription &description);

private:
  std::vector<PipelineDescription> mDescriptions;
  std::vector<PipelineHandle> mRealResources;
};

} // namespace liquid::rhi
