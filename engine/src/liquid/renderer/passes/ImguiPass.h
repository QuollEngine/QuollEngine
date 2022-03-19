#pragma once

#include "../render-graph/RenderGraph.h"
#include "../render-graph/RenderGraphPassBase.h"
#include "../ShaderLibrary.h"
#include "../imgui/ImguiRenderer.h"
#include "liquid/profiler/DebugManager.h"
#include "liquid/profiler/ImguiDebugLayer.h"
#include "liquid/profiler/StatsManager.h"

namespace liquid {

class ImguiPass : public RenderGraphPassBase {
public:
  /**
   * @brief Create imgui pass
   *
   * @param name Pass name
   * @param renderPassId Pass resource ID
   * @param imguiRenderer Imgui renderer
   * @param shaderLibrary Shader library
   * @param debugManager Debug manager
   * @param previousColor Previous color attachment name
   * @param imUpdate Imgui update
   */
  ImguiPass(const String &name, GraphResourceId renderPassId,
            ImguiRenderer &imguiRenderer, ShaderLibrary &shaderLibrary,
            const PhysicalDeviceInformation &deviceInfo,
            StatsManager &statsManager, DebugManager &debugManager,
            const String &previousColor,
            const std::function<void(rhi::TextureHandle)> &imUpdate);
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
  String mPreviousColor;
  ImguiRenderer &mImguiRenderer;
  GraphResourceId mPipelineId = 0;
  ImguiDebugLayer mDebugLayer;
  rhi::TextureHandle mSceneTextureId = rhi::TextureHandle::Invalid;
  std::function<void(rhi::TextureHandle)> mImguiUpdateFn;
};

} // namespace liquid
