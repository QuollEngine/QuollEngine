#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphAttachmentDescription.h"
#include "RenderGraphPipelineDescription.h"

namespace liquid {

class RenderGraph;
class RenderGraphPassBase;

class RenderGraphBuilder {
public:
  /**
   * @brief Create render graph builder
   *
   * @param graph Render graph
   * @param pass Render pass
   */
  RenderGraphBuilder(RenderGraph &graph, RenderGraphPassBase *pass);

  /**
   * @brief Write attachment resource
   *
   * Adds resource ID to render pass output
   *
   * @param name Resource name
   * @return ID associated with resource
   */
  GraphResourceId write(const String &name);

  /**
   * @brief Read attachment resource
   *
   * Reads ID resource from graph and adds it
   * to render pass input.
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
  GraphResourceId create(const RenderGraphPipelineDescription &descriptor);

private:
  RenderGraph &graph;
  RenderGraphPassBase *pass;
};

} // namespace liquid
