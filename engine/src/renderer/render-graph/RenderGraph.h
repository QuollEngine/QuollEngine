#pragma once

#include "renderer/Texture.h"
#include "renderer/RenderCommandList.h"
#include "RenderGraphResource.h"
#include "RenderGraphAttachmentDescriptor.h"
#include "RenderGraphPipelineDescriptor.h"
#include "RenderGraphRegistry.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphInlinePass.h"

namespace liquid {

class RenderGraph;

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
   * @brief Compile graph
   *
   * @return Topologically sorted list of render passes
   */
  std::vector<RenderGraphPassBase *> compile();

  /**
   * @brief Add attachment
   *
   * @param name Attachment name
   * @param attachment Attachment object
   * @return Attachment ID
   */
  GraphResourceId addAttachment(const String &name,
                                const RenderPassAttachment &attachment);

  /**
   * @brief Add swapchain attachment
   *
   * @param name Attachment name
   * @param attachment Attachment object
   * @return Attachment ID
   */
  GraphResourceId
  addSwapchainAttachment(const String &name,
                         const RenderPassSwapchainAttachment &attachment);

  /**
   * @brief Add pipeline
   *
   * @param descriptor Pipeline descriptor
   * @return Pipeline ID
   */
  GraphResourceId addPipeline(const PipelineDescriptor &descriptor);

  /**
   * @brief Get resource from name
   *
   * Generates new ID if resource does not exist
   *
   * @param name Resource name
   * @return Resource ID
   */
  const GraphResourceId getResourceId(const String &name);

  /**
   * @brief Check if resource ID exists for name
   *
   * @param name Resource name
   * @retval true Resource ID exists
   * @retval false Resource ID does not exist
   */
  inline const bool hasResourceId(const String &name) const {
    return resourceMap.find(name) != resourceMap.end();
  }

  /**
   * @brief Check if resource ID has associated attachment
   *
   * @param id Resource ID
   * @retval true Attachment exists for resource ID
   * @retval false No attachment associated with resource ID
   */
  inline const bool hasAttachment(GraphResourceId id) const {
    return attachments.find(id) != attachments.end();
  }

  /**
   * @brief Check if resource ID has associated swapchain attachment
   *
   * @param id Resource ID
   * @retval true Swapchain attachment exists for resource ID
   * @retval false No swapchain attachment associated with resource ID
   */
  inline const bool hasSwapchainAttachment(GraphResourceId id) const {
    return swapchainAttachments.find(id) != swapchainAttachments.end();
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
   * @brief Get attachment resource
   *
   * @param id Resource ID
   * @return Attachment
   */
  inline const RenderPassAttachment &getAttachment(GraphResourceId id) const {
    return attachments.at(id);
  }

  /**
   * @brief Get swapchain attachment resource
   *
   * @param id Resource ID
   * @return Attachment
   */
  inline const RenderPassSwapchainAttachment &
  getSwapchainAttachment(GraphResourceId id) const {
    return swapchainAttachments.at(id);
  }

  /**
   * @brief Get pipeline resource
   *
   * @param id Resource ID
   * @return Pipeline descriptor
   */
  inline const PipelineDescriptor &getPipeline(GraphResourceId id) const {
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
  std::unordered_map<GraphResourceId, RenderPassAttachment> attachments;
  std::unordered_map<GraphResourceId, PipelineDescriptor> pipelines;
  std::unordered_map<String, GraphResourceId> resourceMap;
  std::unordered_map<GraphResourceId, RenderPassSwapchainAttachment>
      swapchainAttachments;
  std::vector<RenderGraphPassBase *> passes;

  GraphResourceId lastId = 0;

  RenderGraphRegistry registry;
};

} // namespace liquid
