#include "liquid/core/Base.h"
#include "ImguiPass.h"
#include <imgui.h>

namespace liquid {

ImguiPass::ImguiPass(const String &name, GraphResourceId renderPassId,
                     ImguiRenderer &imguiRenderer_,
                     ShaderLibrary &shaderLibrary_,
                     const PhysicalDeviceInformation &deviceInfo,
                     StatsManager &statsManager, DebugManager &debugManager,
                     const String &previousColor_,
                     const std::function<void(rhi::TextureHandle)> &imUpdate)
    : RenderGraphPassBase(name, renderPassId), imguiRenderer(imguiRenderer_),
      shaderLibrary(shaderLibrary_),
      debugLayer(deviceInfo, statsManager, debugManager),
      previousColor(previousColor_), imguiUpdateFn(imUpdate) {}

void ImguiPass::buildInternal(RenderGraphBuilder &builder) {
  sceneTextureId = builder.read(previousColor);
  builder.write("SWAPCHAIN");

  pipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary.getShader("__engine.imgui.default.vertex"),
      shaderLibrary.getShader("__engine.imgui.default.fragment"),
      PipelineVertexInputLayout{
          {PipelineVertexInputBinding{0, sizeof(ImDrawVert),
                                      VertexInputRate::Vertex}},
          {PipelineVertexInputAttribute{0, 0, VK_FORMAT_R32G32_SFLOAT,
                                        IM_OFFSETOF(ImDrawVert, pos)},
           PipelineVertexInputAttribute{1, 0, VK_FORMAT_R32G32_SFLOAT,
                                        IM_OFFSETOF(ImDrawVert, uv)},
           PipelineVertexInputAttribute{2, 0, VK_FORMAT_R8G8B8A8_UNORM,
                                        IM_OFFSETOF(ImDrawVert, col)}}

      },
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                         FrontFace::CounterClockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{
          true, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha,
          BlendOp::Add, BlendFactor::One, BlendFactor::OneMinusSrcAlpha,
          BlendOp::Add}}}});
}

void ImguiPass::execute(rhi::RenderCommandList &commandList,
                        RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(pipelineId);
  auto sceneTexture = registry.hasTexture(sceneTextureId)
                          ? registry.getTexture(sceneTextureId)
                          : 0;

  imguiRenderer.beginRendering();

  imguiUpdateFn(sceneTexture);

  debugLayer.render();
  imguiRenderer.endRendering();

  imguiRenderer.draw(commandList, pipeline);
}

} // namespace liquid
