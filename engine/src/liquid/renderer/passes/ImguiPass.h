#pragma once

#include "../render-graph/RenderGraph.h"
#include "../render-graph/RenderGraphPassBase.h"
#include "../ShaderLibrary.h"
#include "../imgui/ImguiRenderer.h"
#include "../vulkan/VulkanAbstraction.h"
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
   * @param backend Render backend
   * @param shaderLibrary Shader library
   * @param debugManager Debug manager
   * @param previousColor Previous color attachment name
   * @param imUpdate Imgui update
   */
  ImguiPass(const String &name, GraphResourceId renderPassId,
            VulkanAbstraction &backend, ShaderLibrary *shaderLibrary,
            const SharedPtr<DebugManager> &debugManager,
            const String &previousColor,
            const std::function<void(TextureHandle)> &imUpdate);
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
  ShaderLibrary *shaderLibrary;
  String previousColor;
  ImguiRenderer imguiRenderer;
  GraphResourceId pipelineId = 0;
  ImguiDebugLayer debugLayer;
  GraphResourceId sceneTextureId = 0;
  std::function<void(TextureHandle)> imguiUpdateFn;
};

} // namespace liquid
