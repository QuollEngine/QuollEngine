#include "liquid/core/Base.h"
#include "ImguiPass.h"
#include <imgui.h>

namespace liquid {

ImguiPass::ImguiPass(const String &name, GraphResourceId renderPassId,
                     ImguiRenderer &imguiRenderer, ShaderLibrary &shaderLibrary,
                     const String &previousColor)
    : RenderGraphPassBase(name, renderPassId), mImguiRenderer(imguiRenderer),
      mShaderLibrary(shaderLibrary), mPreviousColor(previousColor) {}

void ImguiPass::buildInternal(RenderGraphBuilder &builder) {
  builder.read(mPreviousColor);
  builder.write("SWAPCHAIN");

  mPipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.imgui.default.vertex"),
      mShaderLibrary.getShader("__engine.imgui.default.fragment"),
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
  mImguiRenderer.draw(commandList, registry.getPipeline(mPipelineId));
}

} // namespace liquid
