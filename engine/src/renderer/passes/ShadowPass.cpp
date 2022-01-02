#include "core/Base.h"
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

  pipelineId = builder.create(PipelineDescriptor{
      shaderLibrary->getShader("__engine.default.shadowmap.vertex"),
      shaderLibrary->getShader("__engine.default.shadowmap.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise}});
}

void ShadowPass::execute(RenderCommandList &commandList,
                         RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(pipelineId);

  commandList.bindPipeline(pipeline);

  for (auto &shadowMaterial : shadowMaterials) {
    commandList.bindDescriptor(pipeline, 0, shadowMaterial->getDescriptor());

    sceneRenderer.render(commandList, pipeline);
  }
}

} // namespace liquid
