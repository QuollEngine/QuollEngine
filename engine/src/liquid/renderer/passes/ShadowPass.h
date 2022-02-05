#pragma once

#include "../render-graph/RenderGraph.h"
#include "../RenderCommandList.h"
#include "../ShaderLibrary.h"
#include "liquid/entity/EntityContext.h"
#include "../SceneRenderer.h"

namespace liquid {

class ShadowPass : public RenderGraphPassBase {
public:
  /**
   * @brief Create shadow pass
   *
   * @param name Pass name
   * @param renderPassId Pass resource ID
   * @param entityContext Entity context
   * @param shaderLibrary Shader library
   * @param shadowMaterials Shadow materials
   */
  ShadowPass(const String &name, GraphResourceId renderPassId,
             EntityContext &entityContext, ShaderLibrary *shaderLibrary,
             const std::vector<SharedPtr<Material>> &shadowMaterials);
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
  void execute(RenderCommandList &commandList,
               RenderGraphRegistry &registry) override;

private:
  GraphResourceId pipelineId = 0;
  GraphResourceId skinnedPipelineId = 0;
  GraphResourceId shadowMapId = 0;

  ShaderLibrary *shaderLibrary;
  SceneRenderer sceneRenderer;

  std::vector<SharedPtr<Material>> shadowMaterials;
};

} // namespace liquid
