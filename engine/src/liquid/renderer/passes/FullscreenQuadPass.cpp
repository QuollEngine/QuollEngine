#include "liquid/core/Base.h"
#include "FullscreenQuadPass.h"

namespace liquid {

FullscreenQuadPass::FullscreenQuadPass(const String &name,
                                       GraphResourceId renderPassId,
                                       ShaderLibrary &shaderLibrary,
                                       const String &inputDep)
    : RenderGraphPassBase(name, renderPassId), mShaderLibrary(shaderLibrary),
      mInputDep(inputDep) {}

void FullscreenQuadPass::buildInternal(RenderGraphBuilder &builder) {
  mInputTexture = builder.read(mInputDep);
  mPipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.vertex"),
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.fragment"),
      PipelineVertexInputLayout{}, PipelineInputAssembly{},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::CounterClockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  builder.write("SWAPCHAIN");
}

void FullscreenQuadPass::execute(rhi::RenderCommandList &commandList,
                                 RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(mPipelineId);
  commandList.bindPipeline(pipeline);

  rhi::Descriptor descriptor;
  descriptor.bind(0, {mInputTexture},
                  rhi::DescriptorType::CombinedImageSampler);
  commandList.bindDescriptor(pipeline, 0, descriptor);

  commandList.draw(3, 0);
}

} // namespace liquid
