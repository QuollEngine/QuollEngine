#pragma once

#include "liquid/renderer/render-graph/RenderGraphPassBase.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/RenderData.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

class EnvironmentPass : public RenderGraphPassBase {
public:
  /**
   * @brief Create environment pass
   *
   * @param name Pass name
   * @param renderPassId Render pass resource ID
   * @param entityContext Entity context
   * @param shaderLibrary Shader library
   * @param renderData Render data
   */
  EnvironmentPass(const String &name, GraphResourceId renderPassId,
                  EntityContext &entityContext, ShaderLibrary &shaderLibrary,
                  const SharedPtr<RenderData> &renderData);

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   */
  void buildInternal(RenderGraphBuilder &builder) override;

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   */
  void execute(rhi::RenderCommandList &commandList,
               RenderGraphRegistry &registry) override;

private:
  GraphResourceId pipelineId = 0;
  EntityContext &entityContext;
  ShaderLibrary &shaderLibrary;
  SharedPtr<RenderData> renderData;
};

} // namespace liquid
