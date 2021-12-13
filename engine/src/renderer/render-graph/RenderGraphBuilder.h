#pragma once

#include "RenderGraphResource.h"

namespace liquid {

class RenderGraph;
class RenderGraphPassInterface;

class RenderGraphBuilder {
public:
  /**
   * @brief Create render graph builder
   *
   * @param graph Render graph
   * @param pass Render pass
   */
  RenderGraphBuilder(RenderGraph &graph, RenderGraphPassInterface *pass);

  /**
   * @brief Write attachment resource
   *
   * Adds resource to graph and adds resource ID
   * to render pass output
   *
   * @param name Resource name
   * @param attachment Attachment object
   * @return ID associated with resource
   */
  GraphResourceId write(const String &name,
                        const RenderPassAttachment &attachment);

  /**
   * @brief Write swapchain resource
   *
   * @param name Swapchain attachment name
   * @param attachment Swapchain attachment description
   *
   * @return Swapchain resource ID
   */
  GraphResourceId
  writeSwapchain(const String &name,
                 const RenderPassSwapchainAttachment &attachment);

  /**
   * @brief Read attachment resource
   *
   * Reads ID resource from graph and adds it
   * to render pass input. ID will be provided
   * regardless of existence of resource
   *
   * @param name Resource name
   * @return ID associated with resource
   */
  GraphResourceId read(const String &name);

  /**
   * @brief Write pipeline resource
   *
   * Adds resource to graph
   *
   * @param descriptor Pipeline descriptor
   * @return Pipeline resource ID
   */
  GraphResourceId create(const PipelineDescriptor &descriptor);

private:
  RenderGraph &graph;
  RenderGraphPassInterface *pass;
};

} // namespace liquid
