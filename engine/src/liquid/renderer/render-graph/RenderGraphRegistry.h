#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphPassResult.h"

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

class RenderGraphRegistry {
public:
  /**
   * @brief Add texture resource
   *
   * @param resourceId Resource ID
   * @param texture Texture
   */
  void addTexture(GraphResourceId resourceId, TextureHandle texture);

  /**
   * @brief Add pipeline resource
   *
   * @param resourceId Resource ID
   * @param pipeline Pipeline
   */
  void addPipeline(GraphResourceId resourceId, PipelineHandle pipeline);

  /**
   * @brief Add render pass resource
   *
   * @param resourceId Resource ID
   * @param renderPass Render pass
   */
  void addRenderPass(GraphResourceId resourceId,
                     RenderGraphPassResult &&renderPass);

  /**
   * @brief Get texture
   *
   * @param resourceId Resource ID
   * @return Texture
   */
  inline TextureHandle &getTexture(GraphResourceId resourceId) {
    return textures.at(resourceId);
  }

  /**
   * @brief Check if texture resource exists
   *
   * @param resourceId Resource ID
   * @retval true Resource exists
   * @retval false Resource does not exist
   */
  inline bool hasTexture(GraphResourceId resourceId) const {
    return textures.find(resourceId) != textures.end();
  }

  /**
   * @brief Get pipeline
   *
   * @param resourceId Resource ID
   * @return Pipeline
   */
  inline PipelineHandle getPipeline(GraphResourceId resourceId) {
    return pipelines.at(resourceId);
  }

  /**
   * @brief Check if pipeline resource exists
   *
   * @param resourceId Resource ID
   * @retval true Resource exists
   * @retval false Resource does not exist
   */
  inline bool hasPipeline(GraphResourceId resourceId) const {
    return pipelines.find(resourceId) != pipelines.end();
  }

  /**
   * @brief Get render pass
   *
   * @param resourceId Resource ID
   * @return Render pass
   */
  inline RenderGraphPassResult &getRenderPass(GraphResourceId resourceId) {
    return renderPasses.at(resourceId);
  }

  /**
   * @brief Check if render pass resource exists
   *
   * @param resourceId Resource ID
   * @retval true Resource exists
   * @retval false Resource does not exist
   */
  inline bool hasRenderPass(GraphResourceId resourceId) const {
    return renderPasses.find(resourceId) != renderPasses.end();
  }

private:
  std::unordered_map<GraphResourceId, TextureHandle> textures;
  std::unordered_map<GraphResourceId, RenderGraphPassResult> renderPasses;
  std::unordered_map<GraphResourceId, PipelineHandle> pipelines;
};

} // namespace liquid
