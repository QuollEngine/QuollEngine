#include "core/Base.h"
#include "ImguiPass.h"
#include <imgui.h>

namespace liquid {

ImguiPass::ImguiPass(const String &name, GraphResourceId renderPassId,
                     VulkanRenderBackend &backend,
                     ShaderLibrary *shaderLibrary_,
                     const String &previousColor_)
    : RenderGraphPassBase(name, renderPassId),
      imguiRenderer(backend.getWindow(), backend.getVulkanInstance(),
                    backend.getResourceAllocator()),
      shaderLibrary(shaderLibrary_), previousColor(previousColor_) {}

void ImguiPass::buildInternal(RenderGraphBuilder &builder) {
  builder.read("SWAPCHAIN");
  builder.write("SWAPCHAIN");

  pipelineId = builder.create(PipelineDescriptor{
      shaderLibrary->getShader("__engine.imgui.default.vertex"),
      shaderLibrary->getShader("__engine.imgui.default.fragment"),
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

void ImguiPass::execute(RenderCommandList &commandList,
                        RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(pipelineId);
  imguiRenderer.draw(commandList, pipeline);
}

} // namespace liquid
