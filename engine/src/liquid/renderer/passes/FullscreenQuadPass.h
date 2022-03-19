#pragma once

#include "liquid/renderer/render-graph/RenderGraphPassBase.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/RenderData.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

class FullscreenQuadPass : public RenderGraphPassBase {
public:
  /**
   * @brief Create fullscreen quad pass
   *
   * @param name Pass name
   * @param renderPassId Render pass resource ID
   * @param shaderLibrary Shader library
   * @param inputDep Input dependency
   */
  FullscreenQuadPass(const String &name, GraphResourceId renderPassId,
                     ShaderLibrary &shaderLibrary, const String &inputDep);

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
  rhi::TextureHandle mInputTexture = rhi::TextureHandle::Invalid;
  ShaderLibrary &mShaderLibrary;
  String mInputDep;
};

} // namespace liquid
