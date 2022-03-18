#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "RenderGraphResource.h"
#include "RenderGraphAttachmentDescription.h"
#include "RenderGraphPipelineDescription.h"
#include "RenderGraphRegistry.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphInlinePass.h"

namespace liquid {

class RenderGraph {
public:
  RenderGraph() = default;
  RenderGraph(const RenderGraph &) = delete;
  RenderGraph(RenderGraph &&rhs);
  RenderGraph &operator=(const RenderGraph &) = delete;
  RenderGraph &operator=(RenderGraph &&) = delete;

  /**
   * @brief Destroys graph
   *
   * Destroys all passes
   */
  ~RenderGraph();

  /**
   * @brief Add render pass
   *
   * @tparam PassType Pass class type
   * @tparam ...Args Class constructor argument types
   * @param name Pass name
   * @param ...args Class constructor arguments
   */
  template <class PassType, class... Args>
  inline void addPass(const String &name, Args &&...args) {
    addPassInternal(new PassType(name, generateNewId(), args...));
  }

  /**
   * @brief Add inline render pass
   *
   * @tparam TScope Local scope object type
   * @param name Pass name
   * @param builder Builder function
   * @param executor Executor function
   */
  template <class TScope>
  inline void addInlinePass(
      const String &name,
      const typename RenderGraphInlinePass<TScope>::BuilderFn &builder,
      const typename RenderGraphInlinePass<TScope>::ExecutorFn &executor) {
    addPassInternal(new RenderGraphInlinePass<TScope>(name, generateNewId(),
                                                      builder, executor));
  }

  /**
   * @brief Import texture
   *
   * @param name Name of texture
   * @param handle Texture handle
   * @param clearValue Clear value
   */
  void import(const String &name, rhi::TextureHandle handle,
              const std::variant<glm::vec4, DepthStencilClear> &clearValue);

  /**
   * @brief Compile graph
   *
   * @return Topologically sorted list of render passes
   */
  std::vector<RenderGraphPassBase *> compile();

  /**
   * @brief Set swapchain clear color
   *
   * @param color Swapchain color
   */
  void setSwapchainColor(const glm::vec4 &color);

  /**
   * @brief Add pipeline
   *
   * @param descriptor Pipeline descriptor
   * @return Pipeline ID
   */
  GraphResourceId addPipeline(const RenderGraphPipelineDescription &descriptor);

  /**
   * @brief Get resource from name
   *
   * Generates new ID if resource does not exist
   *
   * @param name Resource name
   * @return Resource ID
   */
  rhi::TextureHandle getResourceId(const String &name);

  /**
   * @brief Check if resource ID exists for name
   *
   * @param name Resource name
   * @retval true Resource ID exists
   * @retval false Resource ID does not exist
   */
  inline const bool hasResourceId(const String &name) const {
    return mResourceMap.find(name) != mResourceMap.end();
  }

  /**
   * @brief Get textures
   *
   * @return Textures
   */
  inline const std::unordered_map<rhi::TextureHandle,
                                  std::variant<glm::vec4, DepthStencilClear>> &
  getTextures() const {
    return mTextures;
  }

  /**
   * @brief Get swapchain color
   *
   * @return Swapchain color
   */
  inline const glm::vec4 &getSwapchainColor() const { return swapchainColor; }

  /**
   * @brief Check if resource is swapchain
   *
   * @param id Resource ID
   * @retval true Resource is swapchain
   * @retval false Resource is not swapchain
   */
  inline const bool isSwapchain(rhi::TextureHandle id) const {
    return id == rhi::TextureHandle(1);
  }

  /**
   * @brief Check if resource is a pipeline
   *
   * @param id Resource ID
   * @retval true Resource is a pipeline
   * @retval false Resource is not a pipeline
   */
  inline const bool isPipeline(GraphResourceId id) const {
    return pipelines.find(id) != pipelines.end();
  }

  /**
   * @brief Get pipeline resource
   *
   * @param id Resource ID
   * @return Pipeline descriptor
   */
  inline const RenderGraphPipelineDescription &
  getPipeline(GraphResourceId id) const {
    return pipelines.at(id);
  }

  /**
   * @brief Get resource registry
   *
   * @return Resource registry
   */
  inline RenderGraphRegistry &getResourceRegistry() { return registry; }

  /**
   * @brief Get all render passes
   *
   * @return Render passes
   */
  inline const std::vector<RenderGraphPassBase *> &getRenderPasses() const {
    return passes;
  }

private:
  /**
   * @brief Topological sort function
   *
   * Recursive utility for topological sorting
   *
   * @param index Current index
   * @param visited Visited nodes array
   * @param adjacencyList Adjacency list that represent edges
   * @param output Sort output array
   */
  void topologicalSort(size_t index, std::vector<bool> &visited,
                       const std::vector<std::list<size_t>> &adjacencyList,
                       std::vector<RenderGraphPassBase *> &output);

  /**
   * @brief Generate unique resource ID
   *
   * @return Newly generated resource ID
   */
  GraphResourceId generateNewId();

  /**
   * @brief Add render pass
   *
   * @param pass Render pass
   */
  void addPassInternal(RenderGraphPassBase *pass);

private:
  std::unordered_map<GraphResourceId, RenderGraphPipelineDescription> pipelines;
  std::unordered_map<String, rhi::TextureHandle> mResourceMap{
      {"SWAPCHAIN", rhi::TextureHandle(1)}};

  std::unordered_map<rhi::TextureHandle,
                     std::variant<glm::vec4, DepthStencilClear>>
      mTextures;

  std::vector<RenderGraphPassBase *> passes;

  GraphResourceId lastId = 1;

  RenderGraphRegistry registry;

  glm::vec4 swapchainColor{};
};

} // namespace liquid
