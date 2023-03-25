#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "EditorRenderer.h"

namespace liquid::editor {

EditorRenderer::EditorRenderer(ShaderLibrary &shaderLibrary,
                               IconRegistry &iconRegistry,
                               RenderStorage &renderStorage,
                               rhi::RenderDevice *device)
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

RenderGraphPass &EditorRenderer::attach(RenderGraph &graph) {
  auto &pass = graph.addGraphicsPass("editor-debug");

  auto vEditorGridPipeline = pass.addPipeline(
      {mShaderLibrary.getShader("editor-grid.vert"),
       mShaderLibrary.getShader("editor-grid.frag"),
       {},
       rhi::PipelineInputAssembly{},
       rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                               rhi::FrontFace::Clockwise},
       rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
           true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
           rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
           rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}}});

  auto vSkeletonLinesPipeline = pass.addPipeline(
      {mShaderLibrary.getShader("skeleton-lines.vert"),
       mShaderLibrary.getShader("skeleton-lines.frag"),
       {},
       rhi::PipelineInputAssembly{rhi::PrimitiveTopology::LineList},
       rhi::PipelineRasterizer{rhi::PolygonMode::Line, rhi::CullMode::None,
                               rhi::FrontFace::Clockwise},
       rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
           true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
           rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
           rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}}});

  auto vObjectIconsPipeline = pass.addPipeline(
      {mShaderLibrary.getShader("object-icons.vert"),
       mShaderLibrary.getShader("object-icons.frag"),
       {},
       rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
       rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                               rhi::FrontFace::Clockwise},
       rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
           true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
           rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
           rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}}});

  static const float WireframeLineHeight = 3.0f;

  auto vCollidableShapePipeline = pass.addPipeline(
      {mShaderLibrary.getShader("collidable-shape.vert"),
       mShaderLibrary.getShader("collidable-shape.frag"),
       rhi::PipelineVertexInputLayout::create<Vertex>(),
       rhi::PipelineInputAssembly{rhi::PrimitiveTopology::LineList},
       rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                               rhi::FrontFace::Clockwise, WireframeLineHeight},
       rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
           true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
           rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
           rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}}});

  pass.setExecutor([vEditorGridPipeline, vSkeletonLinesPipeline,
                    vObjectIconsPipeline, vCollidableShapePipeline,
                    this](rhi::RenderCommandList &commandList,
                          const RenderGraphRegistry &registry,
                          uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);
    auto collidableShapePipeline = registry.get(vCollidableShapePipeline);
    auto skeletonLinesPipeline = registry.get(vSkeletonLinesPipeline);
    auto objectIconsPipeline = registry.get(vObjectIconsPipeline);
    auto editorGridPipeline = registry.get(vEditorGridPipeline);

    // Collidable shapes
    if (frameData.isCollidableEntitySelected() &&
        frameData.getCollidableShapeType() != PhysicsGeometryType::Plane) {
      LIQUID_PROFILE_EVENT("EditorPass::CollidableShapes");

      commandList.bindPipeline(collidableShapePipeline);
      commandList.bindDescriptor(collidableShapePipeline, 0,
                                 mRenderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(collidableShapePipeline, 1,
                                 frameData.getBindlessParams().getDescriptor(),
                                 {0});

      auto type = frameData.getCollidableShapeType();

      if (type == PhysicsGeometryType::Box) {
        commandList.bindVertexBuffer(mCollidableCube.buffer.getHandle());
        commandList.draw(mCollidableCube.vertexCount, 0);
      } else if (type == PhysicsGeometryType::Sphere) {
        commandList.bindVertexBuffer(mCollidableSphere.buffer.getHandle());
        commandList.draw(mCollidableSphere.vertexCount, 0);
      } else if (type == PhysicsGeometryType::Capsule) {
        commandList.bindVertexBuffer(mCollidableCapsule.buffer.getHandle());
        commandList.draw(mCollidableCapsule.vertexCount, 0);
      }
    }

    // Editor grid
    {
      LIQUID_PROFILE_EVENT("EditorPass::EditorGrid");

      commandList.bindPipeline(editorGridPipeline);
      commandList.bindDescriptor(editorGridPipeline, 0,
                                 mRenderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(editorGridPipeline, 1,
                                 frameData.getBindlessParams().getDescriptor(),
                                 {0});

      static constexpr uint32_t GridPlaneNumVertices = 6;
      commandList.draw(GridPlaneNumVertices, 0);
    }

    // Skeleton bones
    if (!frameData.getBoneCounts().empty()) {
      LIQUID_PROFILE_EVENT("EditorPass::SkeletonBones");

      commandList.bindPipeline(skeletonLinesPipeline);
      commandList.bindDescriptor(skeletonLinesPipeline, 0,
                                 mRenderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(skeletonLinesPipeline, 1,
                                 frameData.getBindlessParams().getDescriptor(),
                                 {0});

      const auto &numBones = frameData.getBoneCounts();

      for (size_t i = 0; i < numBones.size(); ++i) {
        commandList.draw(numBones.at(i), 0, 1, static_cast<uint32_t>(i));
      }
    }

    // Object gizmos
    {
      LIQUID_PROFILE_EVENT("EditorPass::ObjectGizmos");

      commandList.bindPipeline(objectIconsPipeline);
      commandList.bindDescriptor(objectIconsPipeline, 0,
                                 mRenderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(objectIconsPipeline, 1,
                                 mRenderStorage.getGlobalTexturesDescriptor());
      commandList.bindDescriptor(objectIconsPipeline, 2,
                                 frameData.getBindlessParams().getDescriptor(),
                                 {0});

      uint32_t previousInstance = 0;
      for (auto &[icon, count] : frameData.getGizmoCounts()) {
        auto uIcon = rhi::castHandleToUint(icon);
        commandList.pushConstants(objectIconsPipeline,
                                  rhi::ShaderStage::Fragment, 0,
                                  sizeof(uint32_t), &uIcon);

        commandList.draw(4, 0, count, previousInstance);

        previousInstance += count;
      }
    }
  });

  LOG_DEBUG("Editor renderer attached to graph: " << graph.getName());

  return pass;
}

void EditorRenderer::updateFrameData(EntityDatabase &entityDatabase,
                                     Entity camera, WorkspaceState &state,
                                     uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  LIQUID_PROFILE_EVENT("EditorRenderer::update");
  frameData.clear();

  if (entityDatabase.has<Collidable>(state.selectedEntity)) {
    frameData.setCollidable(
        state.selectedEntity,
        entityDatabase.get<Collidable>(state.selectedEntity),
        entityDatabase.get<WorldTransform>(state.selectedEntity));
  }

  frameData.setActiveCamera(entityDatabase.get<Camera>(camera));

  frameData.setEditorGrid(state.grid);

  for (auto [entity, worldTransform, skeleton] :
       entityDatabase.view<WorldTransform, SkeletonDebug>()) {
    frameData.addSkeleton(worldTransform.worldTransform,
                          skeleton.boneTransforms);
  }

  for (auto [entity, world, light] :
       entityDatabase.view<WorldTransform, DirectionalLight>()) {
    frameData.addGizmo(mIconRegistry.getIcon(EditorIcon::Sun),
                       world.worldTransform);
  }

  for (auto [entity, world, light] :
       entityDatabase.view<WorldTransform, PointLight>()) {
    frameData.addGizmo(mIconRegistry.getIcon(EditorIcon::Light),
                       world.worldTransform);
  }

  for (auto [entity, world, camera] :
       entityDatabase.view<WorldTransform, PerspectiveLens>()) {
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

  using V = Vertex;

  // Box shape
  {
    std::vector<Vertex> CollidableBoxVertices{
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
        {rhi::BufferUsage::Vertex,
         CollidableBoxVertices.size() * sizeof(Vertex),
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
    auto drawUnitCircle = [](std::vector<Vertex> &vertices,
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
    std::vector<Vertex> CollidableSphereVertices;

    drawUnitCircle(CollidableSphereVertices, NumSegments, cZero, cSin, cCos);
    drawUnitCircle(CollidableSphereVertices, NumSegments, cSin, cCos, cZero);

    mCollidableSphere.buffer = mRenderStorage.createBuffer(
        {rhi::BufferUsage::Vertex,
         CollidableSphereVertices.size() * sizeof(Vertex),
         static_cast<const void *>(CollidableSphereVertices.data())});
    mCollidableSphere.vertexCount =
        static_cast<uint32_t>(CollidableSphereVertices.size());
  }

  // Capsule shape
  {
    auto drawUnitHalfCircle = [](std::vector<Vertex> &vertices,
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
    std::vector<Vertex> CollidableCapsuleVertices;

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
        {rhi::BufferUsage::Vertex,
         CollidableCapsuleVertices.size() * sizeof(Vertex),
         static_cast<const void *>(CollidableCapsuleVertices.data())});
    mCollidableCapsule.vertexCount =
        static_cast<uint32_t>(CollidableCapsuleVertices.size());
  }

  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

} // namespace liquid::editor
