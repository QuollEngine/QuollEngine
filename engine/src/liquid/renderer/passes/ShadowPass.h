#pragma once

#include "../render-graph/RenderGraph.h"
#include "../ShaderLibrary.h"
#include "liquid/entity/EntityContext.h"
#include "../SceneRenderer.h"

#include "liquid/rhi/RenderCommandList.h"

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
             EntityContext &entityContext, ShaderLibrary &shaderLibrary,
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
  void execute(rhi::RenderCommandList &commandList,
               RenderGraphRegistry &registry) override;

private:
  GraphResourceId mPipelineId = 0;
  GraphResourceId mSkinnedPipelineId = 0;
  rhi::TextureHandle mShadowMapId = rhi::TextureHandle::Invalid;

  ShaderLibrary &mShaderLibrary;
  SceneRenderer mSceneRenderer;

  std::vector<SharedPtr<Material>> mShadowMaterials;
};

} // namespace liquid
