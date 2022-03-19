#include "liquid/core/Base.h"
#include "ScenePass.h"

namespace liquid {

ScenePass::ScenePass(const String &name, GraphResourceId resourceId,
                     EntityContext &entityContext,
                     ShaderLibrary &shaderLibrary_,
                     const SharedPtr<RenderData> &renderData_,
                     DebugManager &debugManager_)
    : RenderGraphPassBase(name, resourceId), sceneRenderer(entityContext, true),
      shaderLibrary(shaderLibrary_), renderData(renderData_),
      debugManager(debugManager_) {}

void ScenePass::buildInternal(RenderGraphBuilder &builder) {
  builder.write("mainColor");
  builder.write("depthBuffer");

  shadowMapTextureId = builder.read("shadowmap");
  pipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary.getShader("__engine.geometry.default.vertex"),
      shaderLibrary.getShader("__engine.pbr.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  skinnedPipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary.getShader("__engine.geometry.skinned.vertex"),
      shaderLibrary.getShader("__engine.pbr.default.fragment"),
      PipelineVertexInputLayout::create<SkinnedVertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  wireframePipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary.getShader("__engine.geometry.default.vertex"),
      shaderLibrary.getShader("__engine.pbr.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Line, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});
}

void ScenePass::execute(rhi::RenderCommandList &commandList,
                        RenderGraphRegistry &registry) {
  const auto &pipeline = debugManager.getWireframeMode()
                             ? registry.getPipeline(wireframePipelineId)
                             : registry.getPipeline(pipelineId);

  commandList.bindPipeline(pipeline);

  auto cameraBuffer =
      renderData->getScene()->getActiveCamera()->getUniformBuffer();

  rhi::Descriptor sceneDescriptor, sceneDescriptorFragment;

  const auto &iblMaps = renderData->getEnvironmentTextures();

  sceneDescriptor.bind(0, cameraBuffer, rhi::DescriptorType::UniformBuffer);
  sceneDescriptorFragment
      .bind(0, cameraBuffer, rhi::DescriptorType::UniformBuffer)
      .bind(1, renderData->getSceneBuffer(), rhi::DescriptorType::UniformBuffer)
      .bind(2, {shadowMapTextureId}, rhi::DescriptorType::CombinedImageSampler);

  sceneDescriptorFragment
      .bind(3, {iblMaps.at(0), iblMaps.at(1)},
            rhi::DescriptorType::CombinedImageSampler)
      .bind(4, {iblMaps.at(2)}, rhi::DescriptorType::CombinedImageSampler);

  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
  commandList.bindDescriptor(pipeline, 1, sceneDescriptorFragment);

  sceneRenderer.render(commandList, pipeline);

  const auto &skinPipeline = registry.getPipeline(skinnedPipelineId);

  commandList.bindPipeline(skinPipeline);
  commandList.bindDescriptor(skinPipeline, 0, sceneDescriptor);
  commandList.bindDescriptor(skinPipeline, 1, sceneDescriptorFragment);

  sceneRenderer.renderSkinned(commandList, skinPipeline, 3);
}

} // namespace liquid
