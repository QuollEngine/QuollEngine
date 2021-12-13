#pragma once

#include "renderer/Texture.h"
#include "renderer/RenderCommandList.h"
#include "RenderGraphResource.h"
#include "RenderGraphAttachmentDescriptor.h"
#include "RenderGraphPipelineDescriptor.h"
#include "RenderGraphRegistry.h"
#include "RenderGraphBuilder.h"

namespace liquid {

class RenderGraph;
class RenderGraphPassInterface;

class RenderGraphPassInterface {
public:
  /**
   * @brief Create render render graph pass
   *
   * @param renderPass Render pass resource
   */
  RenderGraphPassInterface(GraphResourceId renderPass);

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   * @pure
   */
  virtual void build(RenderGraphBuilder &&builder) = 0;

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @pure
   */
  virtual void execute(RenderCommandList &commandList) = 0;

  /**
   * @brief Get pass name
   *
   * @return Pass name
   * @pure
   */
  virtual const String &getName() const = 0;

  /**
   * @brief Add input resource
   *
   * @param resourceId Input resource ID
   */
  void addInput(GraphResourceId resourceId);

  /**
   * @brief Add output resource
   *
   * @param resourceId Output resource ID
   */
  void addOutput(GraphResourceId resourceId);

  /**
   * @brief Add resource
   *
   * @param resourceId Non attachment resource ID
   */
  void addResource(GraphResourceId resourceId);

  /*
   * @brief Set swapchain relative
   *
   * @param swapchainRelative Swapchain relative flag
   */
  void setSwapchainRelative(bool swapchainRelative);

  /**
   * @brief Get all input resources
   *
   * @return Input resources
   */
  inline const std::vector<GraphResourceId> &getInputs() const {
    return inputs;
  }

  /**
   * @brief Get all output resources
   *
   * @return Output resources
   */
  inline const std::vector<GraphResourceId> &getOutputs() const {
    return outputs;
  }

  /**
   * @brief Get non attachment resources
   *
   * @return Resources
   */
  inline const std::vector<GraphResourceId> &getResources() const {
    return resources;
  }

  /**
   * @brief Get render pass
   *
   * @return Render pass resource
   */
  inline GraphResourceId getRenderPass() const { return renderPass; }

  /**
   * @brief Is swapchain relative
   *
   * @retval true Pass is swapchain relative
   * @retval false Pass is not swapchain relative
   */
  inline bool isSwapchainRelative() const { return swapchainRelative; }

private:
  std::vector<GraphResourceId> inputs;
  std::vector<GraphResourceId> outputs;
  std::vector<GraphResourceId> resources;
  bool swapchainRelative = false;

  GraphResourceId renderPass;
};

template <class TScope>
class RenderGraphPass : public RenderGraphPassInterface {
public:
  using BuilderFn =
      std::function<void(RenderGraphBuilder &builder, TScope &scope)>;
  using ExecutorFn =
      std::function<void(RenderCommandList &commandList, TScope &scope,
                         RenderGraphRegistry &registry)>;

public:
  /**
   * @brief Create render graph pass
   *
   * @param name_ Pass name
   * @param renderPass_ Render pass resource
   * @param builderFn_ Builder function
   * @param executorFn_ Executor function
   * @param registry Resource registry
   */
  RenderGraphPass(const String &name_, GraphResourceId renderPass_,
                  const BuilderFn &builderFn_, const ExecutorFn &executorFn_,
                  RenderGraphRegistry &registry_)
      : RenderGraphPassInterface(renderPass_), name(name_),
        builderFn(builderFn_), executorFn(executorFn_), registry(registry_) {}

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   */
  void build(RenderGraphBuilder &&builder) override {
    if (dirty) {
      builderFn(builder, scope);
      dirty = false;
    }
  }

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   */
  void execute(RenderCommandList &commandList) override {
    executorFn(commandList, scope, registry);
  }

  /**
   * @brief Get pass name
   *
   * @return Pass name
   */
  const String &getName() const override { return name; }

private:
  String name;
  BuilderFn builderFn;
  ExecutorFn executorFn;
  TScope scope{};
  RenderGraphRegistry &registry;
  bool dirty = true;
};

class RenderGraph {
public:
  RenderGraph() = default;
  RenderGraph(const RenderGraph &) = delete;
  RenderGraph(RenderGraph &&) = delete;
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
   * @param pass Render pass
   */
  void addPass(RenderGraphPassInterface *pass);

  /**
   * @brief Add render pass
   *
   * @tparam TScope Local scope object type
   * @param name Pass name
   * @param builder Builder function
   * @param executor Executor function
   */
  template <class TScope>
  inline void
  addPass(const String &name,
          const typename RenderGraphPass<TScope>::BuilderFn &builder,
          const typename RenderGraphPass<TScope>::ExecutorFn &executor) {
    addPass(new RenderGraphPass<TScope>(name, generateNewId(), builder,
                                        executor, registry));
  }

  /**
   * @brief Compile graph
   *
   * @return Topologically sorted list of render passes
   */
  std::vector<RenderGraphPassInterface *> compile();

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
  inline const std::vector<RenderGraphPassInterface *> &
  getRenderPasses() const {
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
                       std::vector<RenderGraphPassInterface *> &output);

  /**
   * @brief Generate unique resource ID
   *
   * @return Newly generated resource ID
   */
  GraphResourceId generateNewId();

private:
  std::unordered_map<GraphResourceId, RenderPassAttachment> attachments;
  std::unordered_map<GraphResourceId, PipelineDescriptor> pipelines;
  std::unordered_map<String, GraphResourceId> resourceMap;
  std::unordered_map<GraphResourceId, RenderPassSwapchainAttachment>
      swapchainAttachments;
  std::vector<RenderGraphPassInterface *> passes;

  GraphResourceId lastId = 0;

  RenderGraphRegistry registry;
};

} // namespace liquid
