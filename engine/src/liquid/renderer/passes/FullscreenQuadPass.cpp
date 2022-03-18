#include "liquid/core/Base.h"
#include "FullscreenQuadPass.h"

namespace liquid {

FullscreenQuadPass::FullscreenQuadPass(const String &name,
                                       GraphResourceId renderPassId,
                                       ShaderLibrary &shaderLibrary_,
                                       const String &inputDep_)
    : RenderGraphPassBase(name, renderPassId), shaderLibrary(shaderLibrary_),
      inputDep(inputDep_) {}

void FullscreenQuadPass::buildInternal(RenderGraphBuilder &builder) {
  inputTexture = builder.read(inputDep);
  pipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary.getShader("__engine.fullscreenQuad.default.vertex"),
      shaderLibrary.getShader("__engine.fullscreenQuad.default.fragment"),
      PipelineVertexInputLayout{}, PipelineInputAssembly{},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::CounterClockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  builder.write("SWAPCHAIN");
}

void FullscreenQuadPass::execute(rhi::RenderCommandList &commandList,
                                 RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(pipelineId);
  commandList.bindPipeline(pipeline);

  rhi::Descriptor descriptor;
  descriptor.bind(0, {inputTexture}, rhi::DescriptorType::CombinedImageSampler);
  commandList.bindDescriptor(pipeline, 0, descriptor);

  commandList.draw(3, 0);
}

} // namespace liquid
