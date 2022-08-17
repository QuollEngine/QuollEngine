#include "liquid/core/Base.h"

#include "EditorRenderer.h"

namespace liquidator {

EditorRenderer::EditorRenderer(liquid::rhi::ResourceRegistry &registry,
                               liquid::ShaderLibrary &shaderLibrary,
                               IconRegistry &iconRegistry,
                               liquid::rhi::RenderDevice *device)
    : mRegistry(registry), mIconRegistry(iconRegistry),
      mShaderLibrary(shaderLibrary), mRenderStorage(device) {

  const auto shadersPath =
      std::filesystem::current_path() / "assets" / "shaders";

  mShaderLibrary.addShader(
      "editor-grid.vert",
      registry.setShader({shadersPath / "editor-grid.vert.spv"}));
  mShaderLibrary.addShader(
      "editor-grid.frag",
      registry.setShader({shadersPath / "editor-grid.frag.spv"}));
  mShaderLibrary.addShader(
      "skeleton-lines.vert",
      registry.setShader({shadersPath / "skeleton-lines.vert.spv"}));
  mShaderLibrary.addShader(
      "skeleton-lines.frag",
      registry.setShader({shadersPath / "skeleton-lines.frag.spv"}));

  mShaderLibrary.addShader(
      "object-icons.vert",
      registry.setShader({shadersPath / "object-icons.vert.spv"}));
  mShaderLibrary.addShader(
      "object-icons.frag",
      registry.setShader({shadersPath / "object-icons.frag.spv"}));
}

liquid::rhi::RenderGraphPass &
EditorRenderer::attach(liquid::rhi::RenderGraph &graph) {
  auto &pass = graph.addPass("editor-debug");

  auto editorGridPipeline = mRegistry.setPipeline(
      {mShaderLibrary.getShader("editor-grid.vert"),
       mShaderLibrary.getShader("editor-grid.frag"),
       {},
       liquid::rhi::PipelineInputAssembly{},
       liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                       liquid::rhi::CullMode::None,
                                       liquid::rhi::FrontFace::Clockwise},
       liquid::rhi::PipelineColorBlend{
           {liquid::rhi::PipelineColorBlendAttachment{
               true, liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
               liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha,
               liquid::rhi::BlendOp::Add}}}});

  auto skeletonLinesPipeline = mRegistry.setPipeline(
      {mShaderLibrary.getShader("skeleton-lines.vert"),
       mShaderLibrary.getShader("skeleton-lines.frag"),
       {},
       liquid::rhi::PipelineInputAssembly{
           liquid::rhi::PrimitiveTopology::LineList},
       liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Line,
                                       liquid::rhi::CullMode::None,
                                       liquid::rhi::FrontFace::Clockwise},
       liquid::rhi::PipelineColorBlend{
           {liquid::rhi::PipelineColorBlendAttachment{
               true, liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
               liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha,
               liquid::rhi::BlendOp::Add}}}});

  auto objectIconsPipeline = mRegistry.setPipeline(
      {mShaderLibrary.getShader("object-icons.vert"),
       mShaderLibrary.getShader("object-icons.frag"),
       {},
       liquid::rhi::PipelineInputAssembly{
           liquid::rhi::PrimitiveTopology::TriangleStrip},
       liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                       liquid::rhi::CullMode::None,
                                       liquid::rhi::FrontFace::Clockwise},
       liquid::rhi::PipelineColorBlend{
           {liquid::rhi::PipelineColorBlendAttachment{
               true, liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
               liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha,
               liquid::rhi::BlendOp::Add}}}});

  pass.addPipeline(editorGridPipeline);
  pass.addPipeline(skeletonLinesPipeline);
  pass.addPipeline(objectIconsPipeline);

  pass.setExecutor([editorGridPipeline, skeletonLinesPipeline,
                    objectIconsPipeline,
                    this](liquid::rhi::RenderCommandList &commandList) {
    // Editor grid
    {
      LIQUID_PROFILE_EVENT("EditorPass::EditorGrid");

      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, mRenderStorage.getActiveCameraBuffer(),
                           liquid::rhi::DescriptorType::UniformBuffer);

      liquid::rhi::Descriptor gridDescriptor;
      gridDescriptor.bind(0, mRenderStorage.getEditorGridBuffer(),
                          liquid::rhi::DescriptorType::UniformBuffer);

      commandList.bindPipeline(editorGridPipeline);
      commandList.bindDescriptor(editorGridPipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(editorGridPipeline, 1, gridDescriptor);

      static constexpr uint32_t GridPlaneNumVertices = 6;
      commandList.draw(GridPlaneNumVertices, 0);
    }

    // Skeleton bones
    if (!mRenderStorage.getBoneCounts().empty()) {
      LIQUID_PROFILE_EVENT("EditorPass::SkeletonBones");

      commandList.bindPipeline(skeletonLinesPipeline);
      auto skeletonBuffer = mRenderStorage.getSkeletonTransforms();

      auto bonesBuffer = mRenderStorage.getSkeletonBoneTransforms();

      const auto &numBones = mRenderStorage.getBoneCounts();

      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, mRenderStorage.getActiveCameraBuffer(),
                           liquid::rhi::DescriptorType::UniformBuffer);
      sceneDescriptor.bind(1, skeletonBuffer,
                           liquid::rhi::DescriptorType::StorageBuffer);
      sceneDescriptor.bind(2, bonesBuffer,
                           liquid::rhi::DescriptorType::StorageBuffer);

      commandList.bindDescriptor(skeletonLinesPipeline, 0, sceneDescriptor);

      for (size_t i = 0; i < numBones.size(); ++i) {
        commandList.draw(numBones.at(i), 0, 1, static_cast<uint32_t>(i));
      }
    }

    // Object gizmos
    {
      LIQUID_PROFILE_EVENT("EditorPass::ObjectGizmos");

      commandList.bindPipeline(objectIconsPipeline);
      liquid::rhi::Descriptor objectListSceneDescriptor;
      objectListSceneDescriptor.bind(
          0, mRenderStorage.getActiveCameraBuffer(),
          liquid::rhi::DescriptorType::UniformBuffer);
      objectListSceneDescriptor.bind(
          1, mRenderStorage.getGizmoTransformsBuffer(),
          liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(objectIconsPipeline, 0,
                                 objectListSceneDescriptor);

      uint32_t previousInstance = 0;
      for (auto &[icon, count] : mRenderStorage.getGizmoCounts()) {
        liquid::rhi::Descriptor iconDescriptor;
        iconDescriptor.bind(0, {icon},
                            liquid::rhi::DescriptorType::CombinedImageSampler);
        commandList.bindDescriptor(objectIconsPipeline, 1, iconDescriptor);

        commandList.draw(4, 0, count, previousInstance);

        previousInstance = count;
      }
    }
  });

  return pass;
}

void EditorRenderer::updateFrameData(liquid::EntityDatabase &entityDatabase,
                                     liquid::Entity camera,
                                     const EditorGrid &editorGrid) {
  LIQUID_PROFILE_EVENT("EditorRenderer::update");
  mRenderStorage.clear();

  mRenderStorage.setActiveCamera(
      entityDatabase.getComponent<liquid::CameraComponent>(camera));

  mRenderStorage.setEditorGrid(editorGrid.getData());

  entityDatabase.iterateEntities<liquid::WorldTransformComponent,
                                 liquid::SkeletonDebugComponent>(
      [this](auto entity, liquid::WorldTransformComponent &worldTransform,
             liquid::SkeletonDebugComponent &skeleton) {
        mRenderStorage.addSkeleton(worldTransform.worldTransform,
                                   skeleton.boneTransforms);
      });

  entityDatabase.iterateEntities<liquid::WorldTransformComponent,
                                 liquid::DirectionalLightComponent>(
      [this](auto entity, const auto &world, const auto &light) {
        mRenderStorage.addGizmo(mIconRegistry.getIcon(EditorIcon::Sun),
                                world.worldTransform);
      });

  entityDatabase.iterateEntities<liquid::WorldTransformComponent,
                                 liquid::PerspectiveLensComponent>(
      [this](auto entity, const auto &world, const auto &camera) {
        static constexpr float NinetyDegreesInRadians = glm::pi<float>() / 2.0f;

        mRenderStorage.addGizmo(mIconRegistry.getIcon(EditorIcon::Camera),
                                glm::rotate(world.worldTransform,
                                            NinetyDegreesInRadians,
                                            glm::vec3(0, 1, 0)));
      });

  mRenderStorage.updateBuffers();
}

} // namespace liquidator
