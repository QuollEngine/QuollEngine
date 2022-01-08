#include "core/Base.h"
#include "ScenePass.h"

namespace liquid {

ScenePass::ScenePass(const String &name, GraphResourceId resourceId,
                     EntityContext &entityContext,
                     ShaderLibrary *shaderLibrary_,
                     const SharedPtr<VulkanRenderData> &renderData_,
                     const SharedPtr<DebugManager> &debugManager_)
    : RenderGraphPassBase(name, resourceId), sceneRenderer(entityContext, true),
      shaderLibrary(shaderLibrary_), renderData(renderData_),
      debugManager(debugManager_) {}

void ScenePass::buildInternal(RenderGraphBuilder &builder) {
  builder.write("SWAPCHAIN");
  builder.write("depthBuffer");

  shadowMapTextureId = builder.read("shadowmap");
  pipelineId = builder.create(PipelineDescriptor{
      shaderLibrary->getShader("__engine.default.pbr.vertex"),
      shaderLibrary->getShader("__engine.default.pbr.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});
  wireframePipelineId = builder.create(PipelineDescriptor{
      shaderLibrary->getShader("__engine.default.pbr.vertex"),
      shaderLibrary->getShader("__engine.default.pbr.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Line, CullMode::None,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});
}

void ScenePass::execute(RenderCommandList &commandList,
                        RenderGraphRegistry &registry) {
  const auto &pipeline = debugManager->getWireframeMode()
                             ? registry.getPipeline(wireframePipelineId)
                             : registry.getPipeline(pipelineId);

  commandList.bindPipeline(pipeline);

  const auto &cameraBuffer =
      renderData->getScene()->getActiveCamera()->getUniformBuffer();

  Descriptor sceneDescriptor, sceneDescriptorFragment;

  const auto &iblMaps = renderData->getEnvironmentTextures();

  sceneDescriptor.bind(0, cameraBuffer, DescriptorType::UniformBuffer);
  sceneDescriptorFragment.bind(0, cameraBuffer, DescriptorType::UniformBuffer)
      .bind(1, renderData->getSceneBuffer(), DescriptorType::UniformBuffer)
      .bind(2, {registry.getTexture(shadowMapTextureId)},
            DescriptorType::CombinedImageSampler);

  sceneDescriptorFragment
      .bind(3, {iblMaps.at(0), iblMaps.at(1)},
            DescriptorType::CombinedImageSampler)
      .bind(4, {iblMaps.at(2)}, DescriptorType::CombinedImageSampler);

  commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
  commandList.bindDescriptor(pipeline, 1, sceneDescriptorFragment);

  sceneRenderer.render(commandList, pipeline);
}

} // namespace liquid
