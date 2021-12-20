#include "core/Base.h"
#include "ScenePass.h"
#include "renderer/vulkan/VulkanPipeline.h"
#include "renderer/vulkan/VulkanHardwareBuffer.h"

namespace liquid {

constexpr glm::vec4 blueishClearValue{0.19f, 0.21f, 0.26f, 1.0f};

ScenePass::ScenePass(const String &name, GraphResourceId resourceId,
                     EntityContext &entityContext,
                     ShaderLibrary *shaderLibrary_,
                     VulkanDescriptorManager *descriptorManager_,
                     const SharedPtr<VulkanRenderData> &renderData_,
                     const SharedPtr<DebugManager> &debugManager_)
    : RenderGraphPassBase(name, resourceId), sceneRenderer(entityContext, true),
      shaderLibrary(shaderLibrary_), descriptorManager(descriptorManager_),
      renderData(renderData_), debugManager(debugManager_) {}

void ScenePass::buildInternal(RenderGraphBuilder &builder) {
  builder.writeSwapchain("mainColor",
                         RenderPassSwapchainAttachment{
                             AttachmentType::Color, AttachmentLoadOp::Clear,
                             AttachmentStoreOp::Store, blueishClearValue});
  builder.writeSwapchain(
      "mainDepth", RenderPassSwapchainAttachment{
                       AttachmentType::Depth, AttachmentLoadOp::Clear,
                       AttachmentStoreOp::Store, DepthStencilClear{1.0f, 0}});
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

  const auto &vulkanMainPipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

  if (renderData->isEnvironmentChanged()) {
    sceneDescriptorSetFragment = VK_NULL_HANDLE;
    sceneDescriptorSet = VK_NULL_HANDLE;
    renderData->cleanEnvironmentChangeFlag();
  }

  if (!sceneDescriptorSet) {
    const auto &cameraBuffer = std::static_pointer_cast<VulkanHardwareBuffer>(
        renderData->getScene()->getActiveCamera()->getUniformBuffer());

    sceneDescriptorSet = descriptorManager->createSceneDescriptorSet(
        cameraBuffer, nullptr, nullptr, {},
        vulkanMainPipeline->getDescriptorLayout(0));

    sceneDescriptorSetFragment = descriptorManager->createSceneDescriptorSet(
        cameraBuffer, renderData->getSceneBuffer(),
        registry.getTexture(shadowMapTextureId),
        renderData->getEnvironmentTextures(),
        vulkanMainPipeline->getDescriptorLayout(1));
  }

  commandList.bindDescriptorSets(pipeline, 0, {sceneDescriptorSet}, {});
  commandList.bindDescriptorSets(pipeline, 1, {sceneDescriptorSetFragment}, {});

  sceneRenderer.render(commandList, pipeline);
}

} // namespace liquid
