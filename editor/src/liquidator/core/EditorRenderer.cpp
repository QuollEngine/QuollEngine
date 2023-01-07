#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "EditorRenderer.h"

namespace liquidator {

EditorRenderer::EditorRenderer(liquid::ShaderLibrary &shaderLibrary,
                               IconRegistry &iconRegistry,
                               liquid::RenderStorage &renderStorage,
                               liquid::rhi::RenderDevice *device)
    : mIconRegistry(iconRegistry), mShaderLibrary(shaderLibrary),
      mRenderStorage(renderStorage),
      mFrameData{EditorRendererFrameData(renderStorage),
                 EditorRendererFrameData(renderStorage)},
      mDevice(device) {

  createCollidableShapes();

  const auto shadersPath =
      std::filesystem::current_path() / "assets" / "shaders";

  mShaderLibrary.addShader(
      "editor-grid.vert",
      mDevice->createShader({shadersPath / "editor-grid.vert.spv"}));
  mShaderLibrary.addShader(
      "editor-grid.frag",
      mDevice->createShader({shadersPath / "editor-grid.frag.spv"}));
  mShaderLibrary.addShader(
      "skeleton-lines.vert",
      mDevice->createShader({shadersPath / "skeleton-lines.vert.spv"}));
  mShaderLibrary.addShader(
      "skeleton-lines.frag",
      mDevice->createShader({shadersPath / "skeleton-lines.frag.spv"}));

  mShaderLibrary.addShader(
      "object-icons.vert",
      mDevice->createShader({shadersPath / "object-icons.vert.spv"}));
  mShaderLibrary.addShader(
      "object-icons.frag",
      mDevice->createShader({shadersPath / "object-icons.frag.spv"}));

  mShaderLibrary.addShader(
      "collidable-shape.vert",
      mDevice->createShader({shadersPath / "collidable-shape.vert.spv"}));
  mShaderLibrary.addShader(
      "collidable-shape.frag",
      mDevice->createShader({shadersPath / "collidable-shape.frag.spv"}));
}

liquid::RenderGraphPass &EditorRenderer::attach(liquid::RenderGraph &graph) {
  auto &pass = graph.addPass("editor-debug");

  auto vEditorGridPipeline = pass.addPipeline(
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

  auto vSkeletonLinesPipeline = pass.addPipeline(
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

  auto vObjectIconsPipeline = pass.addPipeline(
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

  static const float WireframeLineHeight = 3.0f;

  auto vCollidableShapePipeline = pass.addPipeline(
      {mShaderLibrary.getShader("collidable-shape.vert"),
       mShaderLibrary.getShader("collidable-shape.frag"),
       liquid::rhi::PipelineVertexInputLayout::create<liquid::Vertex>(),
       liquid::rhi::PipelineInputAssembly{
           liquid::rhi::PrimitiveTopology::LineList},
       liquid::rhi::PipelineRasterizer{
           liquid::rhi::PolygonMode::Fill, liquid::rhi::CullMode::None,
           liquid::rhi::FrontFace::Clockwise, WireframeLineHeight},
       liquid::rhi::PipelineColorBlend{
           {liquid::rhi::PipelineColorBlendAttachment{
               true, liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
               liquid::rhi::BlendFactor::SrcAlpha,
               liquid::rhi::BlendFactor::DstAlpha,
               liquid::rhi::BlendOp::Add}}}});

  pass.setExecutor([vEditorGridPipeline, vSkeletonLinesPipeline,
                    vObjectIconsPipeline, vCollidableShapePipeline,
                    this](liquid::rhi::RenderCommandList &commandList,
                          const liquid::RenderGraphRegistry &registry,
                          uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);
    auto collidableShapePipeline = registry.get(vCollidableShapePipeline);
    auto skeletonLinesPipeline = registry.get(vSkeletonLinesPipeline);
    auto objectIconsPipeline = registry.get(vObjectIconsPipeline);
    auto editorGridPipeline = registry.get(vEditorGridPipeline);

    // Collidable shapes
    if (frameData.isCollidableEntitySelected() &&
        frameData.getCollidableShapeType() !=
            liquid::PhysicsGeometryType::Plane) {
      LIQUID_PROFILE_EVENT("EditorPass::CollidableShapes");

      commandList.bindPipeline(collidableShapePipeline);
      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, frameData.getActiveCameraBuffer(),
                           liquid::rhi::DescriptorType::UniformBuffer);
      sceneDescriptor.bind(1, frameData.getCollidableParamsBuffer(),
                           liquid::rhi::DescriptorType::UniformBuffer);

      commandList.bindDescriptor(collidableShapePipeline, 0, sceneDescriptor);

      auto type = frameData.getCollidableShapeType();

      if (type == liquid::PhysicsGeometryType::Box) {
        commandList.bindVertexBuffer(mCollidableCube.buffer.getHandle());
        commandList.draw(mCollidableCube.vertexCount, 0);
      } else if (type == liquid::PhysicsGeometryType::Sphere) {
        commandList.bindVertexBuffer(mCollidableSphere.buffer.getHandle());
        commandList.draw(mCollidableSphere.vertexCount, 0);
      } else if (type == liquid::PhysicsGeometryType::Capsule) {
        commandList.bindVertexBuffer(mCollidableCapsule.buffer.getHandle());
        commandList.draw(mCollidableCapsule.vertexCount, 0);
      }
    }

    // Editor grid
    {
      LIQUID_PROFILE_EVENT("EditorPass::EditorGrid");

      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, frameData.getActiveCameraBuffer(),
                           liquid::rhi::DescriptorType::UniformBuffer);

      liquid::rhi::Descriptor gridDescriptor;
      gridDescriptor.bind(0, frameData.getEditorGridBuffer(),
                          liquid::rhi::DescriptorType::UniformBuffer);

      commandList.bindPipeline(editorGridPipeline);
      commandList.bindDescriptor(editorGridPipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(editorGridPipeline, 1, gridDescriptor);

      static constexpr uint32_t GridPlaneNumVertices = 6;
      commandList.draw(GridPlaneNumVertices, 0);
    }

    // Skeleton bones
    if (!frameData.getBoneCounts().empty()) {
      LIQUID_PROFILE_EVENT("EditorPass::SkeletonBones");

      commandList.bindPipeline(skeletonLinesPipeline);
      auto skeletonBuffer = frameData.getSkeletonTransforms();

      auto bonesBuffer = frameData.getSkeletonBoneTransforms();

      const auto &numBones = frameData.getBoneCounts();

      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, frameData.getActiveCameraBuffer(),
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
          0, frameData.getActiveCameraBuffer(),
          liquid::rhi::DescriptorType::UniformBuffer);
      objectListSceneDescriptor.bind(
          1, frameData.getGizmoTransformsBuffer(),
          liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(objectIconsPipeline, 0,
                                 objectListSceneDescriptor);

      uint32_t previousInstance = 0;
      for (auto &[icon, count] : frameData.getGizmoCounts()) {
        liquid::rhi::Descriptor iconDescriptor;
        iconDescriptor.bind(0, {icon},
                            liquid::rhi::DescriptorType::CombinedImageSampler);
        commandList.bindDescriptor(objectIconsPipeline, 1, iconDescriptor);

        commandList.draw(4, 0, count, previousInstance);

        previousInstance = count;
      }
    }
  });

  LOG_DEBUG("Editor renderer attached to graph: " << graph.getName());

  return pass;
}

void EditorRenderer::updateFrameData(liquid::EntityDatabase &entityDatabase,
                                     liquid::Entity camera,
                                     const EditorGrid &editorGrid,
                                     liquid::Entity selectedEntity,
                                     uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  LIQUID_PROFILE_EVENT("EditorRenderer::update");
  frameData.clear();

  if (entityDatabase.has<liquid::Collidable>(selectedEntity)) {
    frameData.setCollidable(
        selectedEntity, entityDatabase.get<liquid::Collidable>(selectedEntity),
        entityDatabase.get<liquid::WorldTransform>(selectedEntity));
  }

  frameData.setActiveCamera(entityDatabase.get<liquid::Camera>(camera));

  frameData.setEditorGrid(editorGrid.getData());

  for (auto [entity, worldTransform, skeleton] :
       entityDatabase.view<liquid::WorldTransform, liquid::SkeletonDebug>()) {
    frameData.addSkeleton(worldTransform.worldTransform,
                          skeleton.boneTransforms);
  }

  for (auto [entity, world, light] :
       entityDatabase
           .view<liquid::WorldTransform, liquid::DirectionalLight>()) {
    frameData.addGizmo(mIconRegistry.getIcon(EditorIcon::Sun),
                       world.worldTransform);
  }

  for (auto [entity, world, camera] :
       entityDatabase.view<liquid::WorldTransform, liquid::PerspectiveLens>()) {
    static constexpr float NinetyDegreesInRadians = glm::pi<float>() / 2.0f;

    frameData.addGizmo(mIconRegistry.getIcon(EditorIcon::Camera),
                       glm::rotate(world.worldTransform, NinetyDegreesInRadians,
                                   glm::vec3(0, 1, 0)));
  }

  frameData.updateBuffers();
}

void EditorRenderer::createCollidableShapes() {
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  static constexpr float Pi = glm::pi<float>();

  using V = liquid::Vertex;

  // Box shape
  {
    std::vector<liquid::Vertex> CollidableBoxVertices{
        // clang-format off
      V{-0.5f, -0.5f, -0.5f}, V{0.5f, -0.5f, -0.5f},
      V{-0.5f, -0.5f, -0.5f}, V{-0.5f, 0.5f, -0.5f},
      V{0.5f, -0.5f, -0.5f}, V{0.5f, 0.5f, -0.5f},
      V{-0.5f, 0.5f, -0.5f}, V{0.5f, 0.5f, -0.5f},

      // Top
      V{-0.5f, -0.5f, -0.5f}, V{-0.5f, -0.5f, 0.5f},
      V{-0.5f, -0.5f, 0.5f}, V{0.5f, -0.5f, 0.5f},
      V{0.5f, -0.5f, -0.5f}, V{0.5f, -0.5f, 0.5f},

      // Front
      V{-0.5f, -0.5f, 0.5f}, V{-0.5f, 0.5f, 0.5f},
      V{-0.5f, 0.5f, 0.5f}, V{0.5f, 0.5f, 0.5f},
      V{0.5f, -0.5f, 0.5f}, V{0.5f, 0.5f, 0.5f},

      // Bottom
      V{0.5f, 0.5f, -0.5f}, V{0.5f, 0.5f, 0.5f},
      V{-0.5f, 0.5f, -0.5f}, V{-0.5f, 0.5f, 0.5f}
        // clang-format on
    };

    mCollidableCube.buffer = mRenderStorage.createBuffer(
        {liquid::rhi::BufferType::Vertex,
         CollidableBoxVertices.size() * sizeof(liquid::Vertex),
         static_cast<const void *>(CollidableBoxVertices.data())});
    mCollidableCube.vertexCount =
        static_cast<uint32_t>(CollidableBoxVertices.size());
  }

  using CalculationFn = std::function<float(float)>;

  static constexpr float Radius = 1.0f;

  auto cSinCenter = [](float center) {
    return [center](float angle) { return Radius * sin(angle) + center; };
  };
  auto cCosCenter = [](float center) {
    return [center](float angle) { return Radius * cos(angle) + center; };
  };

  auto cSin = cSinCenter(0.0f);
  auto cCos = cCosCenter(0.0f);
  auto cZero = [](float angle) { return 0.0f; };

  // Sphere shape
  {
    auto drawUnitCircle = [](std::vector<liquid::Vertex> &vertices,
                             uint32_t numSegments, CalculationFn cX,
                             CalculationFn cY, CalculationFn cZ) {
      const float SegmentDelta = 2.0f * Pi / static_cast<float>(numSegments);
      float segmentAngle = SegmentDelta;

      V start{cX(0.0f), cY(0.0f), cZ(0.0f)};

      vertices.push_back(start);

      for (uint32_t i = 0; i < numSegments; ++i) {
        V vertex{cX(segmentAngle), cY(segmentAngle), cZ(segmentAngle)};

        vertices.push_back(vertex);
        vertices.push_back(vertex);

        segmentAngle += SegmentDelta;
      }

      vertices.push_back(start);
    };

    static constexpr uint32_t NumSegments = 12;
    std::vector<liquid::Vertex> CollidableSphereVertices;

    drawUnitCircle(CollidableSphereVertices, NumSegments, cZero, cSin, cCos);
    drawUnitCircle(CollidableSphereVertices, NumSegments, cSin, cCos, cZero);

    mCollidableSphere.buffer = mRenderStorage.createBuffer(
        {liquid::rhi::BufferType::Vertex,
         CollidableSphereVertices.size() * sizeof(liquid::Vertex),
         static_cast<const void *>(CollidableSphereVertices.data())});
    mCollidableSphere.vertexCount =
        static_cast<uint32_t>(CollidableSphereVertices.size());
  }

  // Capsule shape
  {
    auto drawUnitHalfCircle = [](std::vector<liquid::Vertex> &vertices,
                                 uint32_t numSegments, CalculationFn cX,
                                 CalculationFn cY, CalculationFn cZ,
                                 float circleAngle) {
      const float SegmentDelta = circleAngle / static_cast<float>(numSegments);
      float segmentAngle = SegmentDelta;

      V start{cX(0.0f), cY(0.0f), cZ(0.0f)};

      vertices.push_back(start);

      for (uint32_t i = 0; i < numSegments; ++i) {
        V v{cX(segmentAngle), cY(segmentAngle), cZ(segmentAngle)};

        vertices.push_back(v);
        vertices.push_back(v);

        segmentAngle += SegmentDelta;
      }

      vertices.push_back(vertices.at(vertices.size() - 1));
    };

    static constexpr uint32_t NumSegments = 12;
    std::vector<liquid::Vertex> CollidableCapsuleVertices;

    drawUnitHalfCircle(CollidableCapsuleVertices, NumSegments, cCos,
                       cSinCenter(0.5f), cZero, Pi);
    drawUnitHalfCircle(CollidableCapsuleVertices, NumSegments, cZero,
                       cSinCenter(0.5f), cCos, Pi);

    CollidableCapsuleVertices.push_back(V{-1.0f, 0.5f, 0.0f});
    CollidableCapsuleVertices.push_back(V{-1.0f, -0.5f, 0.0f});

    CollidableCapsuleVertices.push_back(V{0.0f, 0.5f, 1.0f});
    CollidableCapsuleVertices.push_back(V{0.0f, -0.5f, 1.0f});

    CollidableCapsuleVertices.push_back(V{1.0f, 0.5f, 0.0f});
    CollidableCapsuleVertices.push_back(V{1.0f, -0.5f, 0.0f});

    CollidableCapsuleVertices.push_back(V{0.0f, 0.5f, -1.0f});
    CollidableCapsuleVertices.push_back(V{0.0f, -0.5f, -1.0f});

    drawUnitHalfCircle(CollidableCapsuleVertices, NumSegments, cCos,
                       cSinCenter(-0.5f), cZero, -Pi);
    drawUnitHalfCircle(CollidableCapsuleVertices, NumSegments, cZero,
                       cSinCenter(-0.5f), cCos, -Pi);

    mCollidableCapsule.buffer = mRenderStorage.createBuffer(
        {liquid::rhi::BufferType::Vertex,
         CollidableCapsuleVertices.size() * sizeof(liquid::Vertex),
         static_cast<const void *>(CollidableCapsuleVertices.data())});
    mCollidableCapsule.vertexCount =
        static_cast<uint32_t>(CollidableCapsuleVertices.size());
  }

  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

} // namespace liquidator
