#include "liquid/core/Base.h"
#include "ShadowPass.h"

namespace liquid {

ShadowPass::ShadowPass(const String &name, GraphResourceId renderPass,
                       EntityContext &entityContext,
                       ShaderLibrary &shaderLibrary,
                       const std::vector<SharedPtr<Material>> &shadowMaterials)
    : RenderGraphPassBase(name, renderPass), mShaderLibrary(shaderLibrary),
      mSceneRenderer(entityContext, false), mShadowMaterials(shadowMaterials) {}

void ShadowPass::buildInternal(RenderGraphBuilder &builder) {
  mShadowMapId = builder.write("shadowmap");

  mPipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.shadowmap.default.vertex"),
      mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise}});

  mSkinnedPipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.shadowmap.skinned.vertex"),
      mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
      PipelineVertexInputLayout::create<SkinnedVertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise}});
}

void ShadowPass::execute(rhi::RenderCommandList &commandList,
                         RenderGraphRegistry &registry) {
  auto pipeline = registry.getPipeline(mPipelineId);

  commandList.bindPipeline(pipeline);

  for (auto &shadowMaterial : mShadowMaterials) {
    commandList.bindDescriptor(pipeline, 0, shadowMaterial->getDescriptor());

    mSceneRenderer.render(commandList, pipeline);
  }

  auto skinnedPipeline = registry.getPipeline(mSkinnedPipelineId);
  commandList.bindPipeline(skinnedPipeline);

  for (auto &shadowMaterial : mShadowMaterials) {

    commandList.bindDescriptor(skinnedPipeline, 0,
                               shadowMaterial->getDescriptor());

    mSceneRenderer.renderSkinned(commandList, skinnedPipeline, 1);
  }
}

} // namespace liquid
