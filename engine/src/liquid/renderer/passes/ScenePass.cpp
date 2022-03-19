#include "liquid/core/Base.h"
#include "ScenePass.h"

namespace liquid {

ScenePass::ScenePass(const String &name, GraphResourceId resourceId,
                     EntityContext &entityContext, ShaderLibrary &shaderLibrary,
                     const SharedPtr<RenderData> &renderData,
                     DebugManager &debugManager)
    : RenderGraphPassBase(name, resourceId),
      mSceneRenderer(entityContext, true), mShaderLibrary(shaderLibrary),
      mRenderData(renderData), mDebugManager(debugManager) {}

void ScenePass::buildInternal(RenderGraphBuilder &builder) {
  builder.write("mainColor");
  builder.write("depthBuffer");

  mShadowMapTextureId = builder.read("shadowmap");
  mPipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.geometry.default.vertex"),
      mShaderLibrary.getShader("__engine.pbr.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  mSkinnedPipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.geometry.skinned.vertex"),
      mShaderLibrary.getShader("__engine.pbr.default.fragment"),
      PipelineVertexInputLayout::create<SkinnedVertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  mWireframePipelineId = builder.create(RenderGraphPipelineDescription{
      mShaderLibrary.getShader("__engine.geometry.default.vertex"),
      mShaderLibrary.getShader("__engine.pbr.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Line, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});
}

void ScenePass::execute(rhi::RenderCommandList &commandList,
                        RenderGraphRegistry &registry) {
  const auto &pipeline = mDebugManager.getWireframeMode()
                             ? registry.getPipeline(mWireframePipelineId)
                             : registry.getPipeline(mPipelineId);

  commandList.bindPipeline(pipeline);

  auto cameraBuffer = mRenderData->getScene()->getActiveCamera()->getBuffer();

  rhi::Descriptor sceneDescriptor, sceneDescriptorFragment;

  const auto &iblMaps = mRenderData->getEnvironmentTextures();

  sceneDescriptor.bind(0, cameraBuffer, rhi::DescriptorType::UniformBuffer);
  sceneDescriptorFragment
      .bind(0, cameraBuffer, rhi::DescriptorType::UniformBuffer)
      .bind(1, mRenderData->getSceneBuffer(),
            rhi::DescriptorType::UniformBuffer)
      .bind(2, {mShadowMapTextureId},
            rhi::DescriptorType::CombinedImageSampler);

  sceneDescriptorFragment
      .bind(3, {iblMaps.at(0), iblMaps.at(1)},
            rhi::DescriptorType::CombinedImageSampler)
      .bind(4, {iblMaps.at(2)}, rhi::DescriptorType::CombinedImageSampler);

  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
  commandList.bindDescriptor(pipeline, 1, sceneDescriptorFragment);

  mSceneRenderer.render(commandList, pipeline);

  const auto &skinPipeline = registry.getPipeline(mSkinnedPipelineId);

  commandList.bindPipeline(skinPipeline);
  commandList.bindDescriptor(skinPipeline, 0, sceneDescriptor);
  commandList.bindDescriptor(skinPipeline, 1, sceneDescriptorFragment);

  mSceneRenderer.renderSkinned(commandList, skinPipeline, 3);
}

} // namespace liquid
