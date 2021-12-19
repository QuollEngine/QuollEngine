#pragma once

#include "../render-graph/RenderGraph.h"
#include "../render-graph/RenderGraphPassBase.h"
#include "../ShaderLibrary.h"
#include "../imgui/ImguiRenderer.h"
#include "../vulkan/VulkanRenderBackend.h"

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
   * @param previousColor Previous color attachment name
   */
  ImguiPass(const String &name, GraphResourceId renderPassId,
            VulkanRenderBackend &backend, ShaderLibrary *shaderLibrary,
            const String &previousColor);
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
};

} // namespace liquid
