#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphPassResult.h"

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

class RenderGraphRegistry {
public:
  /**
   * @brief Add pipeline resource
   *
   * @param resourceId Resource ID
   * @param pipeline Pipeline
   */
  void addPipeline(GraphResourceId resourceId, rhi::PipelineHandle pipeline);

  /**
   * @brief Add render pass resource
   *
   * @param resourceId Resource ID
   * @param renderPass Render pass
   */
  void addRenderPass(GraphResourceId resourceId,
                     RenderGraphPassResult &&renderPass);

  /**
   * @brief Get pipeline
   *
   * @param resourceId Resource ID
   * @return Pipeline
   */
  inline rhi::PipelineHandle getPipeline(GraphResourceId resourceId) {
    return mPipelines.at(resourceId);
  }

  /**
   * @brief Check if pipeline resource exists
   *
   * @param resourceId Resource ID
   * @retval true Resource exists
   * @retval false Resource does not exist
   */
  inline bool hasPipeline(GraphResourceId resourceId) const {
    return mPipelines.find(resourceId) != mPipelines.end();
  }

  /**
   * @brief Get render pass
   *
   * @param resourceId Resource ID
   * @return Render pass
   */
  inline RenderGraphPassResult &getRenderPass(GraphResourceId resourceId) {
    return mRenderPasses.at(resourceId);
  }

  /**
   * @brief Check if render pass resource exists
   *
   * @param resourceId Resource ID
   * @retval true Resource exists
   * @retval false Resource does not exist
   */
  inline bool hasRenderPass(GraphResourceId resourceId) const {
    return mRenderPasses.find(resourceId) != mRenderPasses.end();
  }

private:
  std::unordered_map<GraphResourceId, RenderGraphPassResult> mRenderPasses;
  std::unordered_map<GraphResourceId, rhi::PipelineHandle> mPipelines;
};

} // namespace liquid
