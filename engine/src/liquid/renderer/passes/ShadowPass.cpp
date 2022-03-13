#include "liquid/core/Base.h"
#include "ShadowPass.h"

namespace liquid {

ShadowPass::ShadowPass(const String &name, GraphResourceId renderPass,
                       EntityContext &entityContext,
                       ShaderLibrary *shaderLibrary_,
                       const std::vector<SharedPtr<Material>> &shadowMaterials_)
    : RenderGraphPassBase(name, renderPass), shaderLibrary(shaderLibrary_),
      sceneRenderer(entityContext, false), shadowMaterials(shadowMaterials_) {}

void ShadowPass::buildInternal(RenderGraphBuilder &builder) {
  shadowMapId = builder.write("shadowmap");

  pipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary->getShader("__engine.shadowmap.default.vertex"),
      shaderLibrary->getShader("__engine.shadowmap.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise}});

  skinnedPipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary->getShader("__engine.shadowmap.skinned.vertex"),
      shaderLibrary->getShader("__engine.shadowmap.default.fragment"),
      PipelineVertexInputLayout::create<SkinnedVertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise}});
}

void ShadowPass::execute(RenderCommandList &commandList,
                         RenderGraphRegistry &registry) {
  auto pipeline = registry.getPipeline(pipelineId);

  commandList.bindPipeline(pipeline);

  for (auto &shadowMaterial : shadowMaterials) {
    commandList.bindDescriptor(pipeline, 0, shadowMaterial->getDescriptor());

    sceneRenderer.render(commandList, pipeline);
  }

  auto skinnedPipeline = registry.getPipeline(skinnedPipelineId);
  commandList.bindPipeline(skinnedPipeline);

  for (auto &shadowMaterial : shadowMaterials) {

    commandList.bindDescriptor(skinnedPipeline, 0,
                               shadowMaterial->getDescriptor());

    sceneRenderer.renderSkinned(commandList, skinnedPipeline, 1);
  }
}

} // namespace liquid
