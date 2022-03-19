#pragma once

#include "liquid/renderer/render-graph/RenderGraph.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/SceneRenderer.h"
#include "liquid/renderer/RenderData.h"

#include "liquid/rhi/Descriptor.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/profiler/DebugManager.h"

namespace liquid {

class ScenePass : public RenderGraphPassBase {
public:
  /**
   * @brief Create scene pass
   *
   * @param name Pass name
   * @param renderPassId Pass resource ID
   * @param entityContext Entity context
   * @param shaderLibrary Shader library
   * @param renderData Render data
   * @param debugManager Debug manager
   */
  ScenePass(const String &name, GraphResourceId renderPassId,
            EntityContext &entityContext, ShaderLibrary &shaderLibrary,
            const SharedPtr<RenderData> &renderData,
            DebugManager &debugManager);

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
  ShaderLibrary &mShaderLibrary;
  SceneRenderer mSceneRenderer;

  GraphResourceId mPipelineId = 0;
  GraphResourceId mWireframePipelineId = 0;
  rhi::TextureHandle mShadowMapTextureId = rhi::TextureHandle::Invalid;

  GraphResourceId mSkinnedPipelineId = 0;

  SharedPtr<RenderData> mRenderData;
  DebugManager mDebugManager;
};

} // namespace liquid
