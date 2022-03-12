#pragma once

#include "liquid/rhi/RenderCommandList.h"

#include "RenderGraphResource.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphRegistry.h"

#include "RenderGraphAttachmentDescriptor.h"

namespace liquid {

class RenderGraphPassBase {
public:
  /**
   * @brief Create render graph pass
   *
   * @param name Render pass name
   * @param renderPass Render pass resource
   */
  RenderGraphPassBase(const String &name, GraphResourceId renderPass);

  RenderGraphPassBase(const RenderGraphPassBase &) = delete;
  RenderGraphPassBase(RenderGraphPassBase &&) = delete;
  RenderGraphPassBase &operator=(const RenderGraphPassBase &) = delete;
  RenderGraphPassBase &operator=(RenderGraphPassBase &&) = delete;

  /**
   * @brief Default virtual destructor
   */
  virtual ~RenderGraphPassBase() = default;

  /**
   * @brief Build pass
   *
   * Checks for dirty flag and calls buildInternal
   *
   * @param builder Graph builder
   */
  void build(RenderGraphBuilder &&builder);

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   * @pure
   */
  virtual void buildInternal(RenderGraphBuilder &builder) = 0;

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   * @pure
   */
  virtual void execute(RenderCommandList &commandList,
                       RenderGraphRegistry &registry) = 0;

  /**
   * @brief Get pass name
   *
   * @return Pass name
   */
  inline const String &getName() const { return name; }

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
   * @param attachment Attachment
   */
  void addOutput(GraphResourceId resourceId,
                 const RenderPassAttachment &attachment);

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
  inline std::unordered_map<GraphResourceId, RenderPassAttachment> &
  getOutputs() {
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

  /**
   * @brief Check if pass is dirty and has to be rebuild
   *
   * @retval true Pass is dirty
   * @retval false Pass is not dirty
   */
  inline bool isDirty() const { return dirty; }

private:
  std::vector<GraphResourceId> inputs;
  std::vector<GraphResourceId> resources;
  std::unordered_map<GraphResourceId, RenderPassAttachment> outputs;

  bool swapchainRelative = false;

  GraphResourceId renderPass;
  String name;
  bool dirty = true;
};

} // namespace liquid
