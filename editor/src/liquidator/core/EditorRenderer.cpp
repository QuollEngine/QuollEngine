#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "EditorRenderer.h"

namespace liquid::editor {

EditorRenderer::EditorRenderer(IconRegistry &iconRegistry,
                               RenderStorage &renderStorage,
                               rhi::RenderDevice *device)
    : mIconRegistry(iconRegistry), mRenderStorage(renderStorage),
      mFrameData{EditorRendererFrameData(renderStorage),
                 EditorRendererFrameData(renderStorage)},
      mDevice(device) {

  createCollidableShapes();

  const auto shadersPath =
      std::filesystem::current_path() / "assets" / "shaders";

  mRenderStorage.createShader("editor-grid.vert",
                              {shadersPath / "editor-grid.vert.spv"});
  mRenderStorage.createShader("editor-grid.frag",
                              {shadersPath / "editor-grid.frag.spv"});
  mRenderStorage.createShader("skeleton-lines.vert",
                              {shadersPath / "skeleton-lines.vert.spv"});
  mRenderStorage.createShader("skeleton-lines.frag",
                              {shadersPath / "skeleton-lines.frag.spv"});

  mRenderStorage.createShader("object-icons.vert",
                              {shadersPath / "object-icons.vert.spv"});
  mRenderStorage.createShader("object-icons.frag",
                              {shadersPath / "object-icons.frag.spv"});

  mRenderStorage.createShader("collidable-shape.vert",
                              {shadersPath / "collidable-shape.vert.spv"});
  mRenderStorage.createShader("collidable-shape.frag",
                              {shadersPath / "collidable-shape.frag.spv"});

  mRenderStorage.createShader("outline-sprite.vert",
                              {shadersPath / "outline-sprite.vert.spv"});
  mRenderStorage.createShader("outline-geometry.vert",
                              {shadersPath / "outline-geometry.vert.spv"});
  mRenderStorage.createShader(
      "outline-skinned-geometry.vert",
      {shadersPath / "outline-skinned-geometry.vert.spv"});
  mRenderStorage.createShader("outline-color.frag",
                              {shadersPath / "outline-color.frag.spv"});
}

void EditorRenderer::attach(RenderGraph &graph,
                            const SceneRenderPassData &scenePassData) {
  // editor debug
  {
    auto &editorDebugPass = graph.addGraphicsPass("editorDebug");
    editorDebugPass.write(scenePassData.sceneColor, AttachmentType::Color,
                          glm::vec4(0.0));
    editorDebugPass.write(scenePassData.depthBuffer, AttachmentType::Depth,
                          rhi::DepthStencilClear{1.0f, 0});
    editorDebugPass.write(scenePassData.sceneColorResolved,
                          AttachmentType::Resolve, glm::vec4(0.0));

    auto editorGridPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("editor-grid.vert"),
         mRenderStorage.getShader("editor-grid.frag"),
         {},
         rhi::PipelineInputAssembly{},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
             rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
             rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}},
         {},
         {},
         "editor grid"});

    auto skeletonLinesPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("skeleton-lines.vert"),
         mRenderStorage.getShader("skeleton-lines.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::LineList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Line, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
             rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
             rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}},
         {},
         {},
         "skeleton lines"});

    auto objectIconsPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("object-icons.vert"),
         mRenderStorage.getShader("object-icons.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
             rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
             rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}},
         {},
         {},
         "object icons"});

    static const float WireframeLineHeight = 3.0f;

    auto collidableShapePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("collidable-shape.vert"),
         mRenderStorage.getShader("collidable-shape.frag"),
         rhi::PipelineVertexInputLayout::create<Vertex>(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::LineList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise,
                                 WireframeLineHeight},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::DstAlpha,
             rhi::BlendOp::Add, rhi::BlendFactor::SrcAlpha,
             rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}},
         {},
         {},
         "collidable shape"});

    editorDebugPass.addPipeline(editorGridPipeline);
    editorDebugPass.addPipeline(skeletonLinesPipeline);
    editorDebugPass.addPipeline(objectIconsPipeline);
    editorDebugPass.addPipeline(collidableShapePipeline);

    editorDebugPass.setExecutor([editorGridPipeline, skeletonLinesPipeline,
                                 objectIconsPipeline, collidableShapePipeline,
                                 this](rhi::RenderCommandList &commandList,
                                       uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      std::array<uint32_t, 1> offsets{0};
      // Collidable shapes
      if (frameData.isCollidableEntitySelected() &&
          frameData.getCollidableShapeType() != PhysicsGeometryType::Plane) {
        LIQUID_PROFILE_EVENT("EditorPass::CollidableShapes");

        commandList.bindPipeline(collidableShapePipeline);
        commandList.bindDescriptor(
            collidableShapePipeline, 0,
            frameData.getBindlessParams().getDescriptor(), offsets);

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
        commandList.bindDescriptor(
            editorGridPipeline, 0,
            frameData.getBindlessParams().getDescriptor(), offsets);

        static constexpr uint32_t GridPlaneNumVertices = 6;
        commandList.draw(GridPlaneNumVertices, 0);
      }

      // Skeleton bones
      if (!frameData.getBoneCounts().empty()) {
        LIQUID_PROFILE_EVENT("EditorPass::SkeletonBones");

        commandList.bindPipeline(skeletonLinesPipeline);
        commandList.bindDescriptor(
            skeletonLinesPipeline, 0,
            frameData.getBindlessParams().getDescriptor(), offsets);

        const auto &numBones = frameData.getBoneCounts();

        for (size_t i = 0; i < numBones.size(); ++i) {
          commandList.draw(numBones.at(i), 0, 1, static_cast<uint32_t>(i));
        }
      }

      // Object gizmos
      {
        LIQUID_PROFILE_EVENT("EditorPass::ObjectGizmos");

        commandList.bindPipeline(objectIconsPipeline);
        commandList.bindDescriptor(
            objectIconsPipeline, 0,
            mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindDescriptor(
            objectIconsPipeline, 1,
            frameData.getBindlessParams().getDescriptor(), offsets);

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
  }

  // outlines
  {
    auto depthBuffer = graph.create([scenePassData](auto width, auto height) {
      rhi::TextureDescription description{};
      description.usage = rhi::TextureUsage::Depth |
                          rhi::TextureUsage::Stencil |
                          rhi::TextureUsage::Sampled;
      description.width = width;
      description.height = height;
      description.layerCount = 1;
      description.samples = scenePassData.sampleCount;
      description.format = rhi::Format::Depth32FloatStencil8Uint;
      description.debugName = "Editor depth stencil for outline";
      return description;
    });

    auto &outlinePass = graph.addGraphicsPass("outlinePass");
    outlinePass.write(scenePassData.sceneColor, AttachmentType::Color,
                      glm::vec4(0.0));
    outlinePass.write(depthBuffer, AttachmentType::Depth,
                      rhi::DepthStencilClear{1.0f, 0});
    outlinePass.write(scenePassData.sceneColorResolved, AttachmentType::Resolve,
                      glm::vec4(0.0));

    static constexpr uint32_t MaskAll = 0xFF;
    rhi::PipelineDepthStencil outlineWritePipelineStencil{
        .depthTest = true,
        .depthWrite = true,
        .stencilTest = true,
        .front = {.failOp = rhi::StencilOp::Keep,
                  .passOp = rhi::StencilOp::Replace,
                  .depthFailOp = rhi::StencilOp::Keep,
                  .compareOp = rhi::CompareOp::Always,
                  .compareMask = 0x00,
                  .writeMask = MaskAll,
                  .reference = 1},
        .back = {.failOp = rhi::StencilOp::Keep,
                 .passOp = rhi::StencilOp::Replace,
                 .depthFailOp = rhi::StencilOp::Keep,
                 .compareOp = rhi::CompareOp::Always,
                 .compareMask = 0x00,
                 .writeMask = MaskAll,
                 .reference = 1}};

    rhi::PipelineDepthStencil outlinePipelineStencil{
        .depthTest = false,
        .depthWrite = true,
        .stencilTest = true,
        .front = {.failOp = rhi::StencilOp::Keep,
                  .passOp = rhi::StencilOp::Replace,
                  .depthFailOp = rhi::StencilOp::Keep,
                  .compareOp = rhi::CompareOp::NotEqual,
                  .compareMask = MaskAll,
                  .writeMask = 0x00,
                  .reference = 1},
        .back = {.failOp = rhi::StencilOp::Keep,
                 .passOp = rhi::StencilOp::Replace,
                 .depthFailOp = rhi::StencilOp::Keep,
                 .compareOp = rhi::CompareOp::NotEqual,
                 .compareMask = MaskAll,
                 .writeMask = 0x00,
                 .reference = 1}};

    auto outlineSpriteStencilWritePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-sprite.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add}}},
         outlineWritePipelineStencil});

    auto outlineSpritePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-sprite.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add}}},
         outlinePipelineStencil});

    auto outlineGeometryStencilWritePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-geometry.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         rhi::PipelineVertexInputLayout::create<Vertex>(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add}}},
         outlineWritePipelineStencil});

    auto outlineGeometryPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-geometry.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         rhi::PipelineVertexInputLayout::create<Vertex>(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add}}},
         outlinePipelineStencil});

    auto outlineSkinnedGeometryStencilWritePipeline =
        mRenderStorage.addPipeline(
            {mRenderStorage.getShader("outline-skinned-geometry.vert"),
             mRenderStorage.getShader("outline-color.frag"),
             rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
             rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
             rhi::PipelineRasterizer{rhi::PolygonMode::Fill,
                                     rhi::CullMode::None,
                                     rhi::FrontFace::Clockwise},
             rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
                 true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
                 rhi::BlendOp::Add, rhi::BlendFactor::Zero,
                 rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}},
             outlineWritePipelineStencil});

    auto outlineSkinnedGeometryPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-skinned-geometry.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
         outlinePipelineStencil});

    outlinePass.addPipeline(outlineSpriteStencilWritePipeline);
    outlinePass.addPipeline(outlineSpritePipeline);
    outlinePass.addPipeline(outlineGeometryStencilWritePipeline);
    outlinePass.addPipeline(outlineGeometryPipeline);
    outlinePass.addPipeline(outlineSkinnedGeometryStencilWritePipeline);
    outlinePass.addPipeline(outlineSkinnedGeometryPipeline);

    outlinePass.setExecutor(
        [outlineSpriteStencilWritePipeline, outlineSpritePipeline,
         outlineGeometryStencilWritePipeline, outlineGeometryPipeline,
         outlineSkinnedGeometryStencilWritePipeline,
         outlineSkinnedGeometryPipeline,
         this](rhi::RenderCommandList &commandList, uint32_t frameIndex) {
          static constexpr glm::vec4 OutlineColor{1.0f, 0.26f, 0.0f, 1.0f};
          static constexpr float OutlineScale = 1.02f;

          auto &frameData = mFrameData.at(frameIndex);

          auto spriteEnd =
              static_cast<uint32_t>(frameData.getOutlineSpriteEnd());
          auto meshEnd = static_cast<uint32_t>(frameData.getOutlineMeshEnd());
          auto skinnedMeshEnd =
              static_cast<uint32_t>(frameData.getOutlineSkinnedMeshEnd());

          renderSpriteOutlines(commandList, frameData,
                               outlineSpriteStencilWritePipeline, 0, spriteEnd,
                               glm::vec4{0.0f}, 1.0f);
          renderSpriteOutlines(commandList, frameData, outlineSpritePipeline, 0,
                               spriteEnd, OutlineColor, OutlineScale);

          renderMeshOutlines(commandList, frameData,
                             outlineGeometryStencilWritePipeline, spriteEnd,
                             meshEnd, glm::vec4{0.0f}, 1.0f);
          renderMeshOutlines(commandList, frameData, outlineGeometryPipeline,
                             spriteEnd, meshEnd, OutlineColor, OutlineScale);

          renderMeshOutlines(commandList, frameData,
                             outlineSkinnedGeometryStencilWritePipeline,
                             meshEnd, skinnedMeshEnd, glm::vec4{0.0f}, 1.0f);
          renderMeshOutlines(commandList, frameData,
                             outlineSkinnedGeometryPipeline, meshEnd,
                             skinnedMeshEnd, OutlineColor, OutlineScale);
        });
  }

  LOG_DEBUG("Editor renderer attached to graph: " << graph.getName());
}

void EditorRenderer::updateFrameData(EntityDatabase &entityDatabase,
                                     Entity camera, WorkspaceState &state,
                                     uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  LIQUID_PROFILE_EVENT("EditorRenderer::update");
  frameData.clear();

  if (entityDatabase.exists(state.selectedEntity)) {
    if (entityDatabase.has<Sprite>(state.selectedEntity)) {
      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);
      frameData.addSpriteOutline(world.worldTransform);
    } else if (entityDatabase.has<Mesh>(state.selectedEntity)) {
      auto handle = entityDatabase.get<Mesh>(state.selectedEntity).handle;

      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);

      const auto &data = state.assetRegistry.getMeshes().getAsset(handle).data;
      frameData.addMeshOutline(data, world.worldTransform);
    } else if (entityDatabase.has<SkinnedMesh>(state.selectedEntity) &&
               entityDatabase.has<Skeleton>(state.selectedEntity)) {
      auto handle =
          entityDatabase.get<SkinnedMesh>(state.selectedEntity).handle;

      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);
      const auto &data =
          state.assetRegistry.getSkinnedMeshes().getAsset(handle).data;

      const auto &skeleton = entityDatabase.get<Skeleton>(state.selectedEntity)
                                 .jointFinalTransforms;

      frameData.addSkinnedMeshOutline(data, skeleton, world.worldTransform);
    }
  }

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

void EditorRenderer::renderSpriteOutlines(rhi::RenderCommandList &commandList,
                                          EditorRendererFrameData &frameData,
                                          rhi::PipelineHandle pipeline,
                                          uint32_t instanceStart,
                                          uint32_t instanceEnd, glm::vec4 color,
                                          float scale) {
  std::array<uint32_t, 1> offsets{0};

  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(
      pipeline, 0, frameData.getBindlessParams().getDescriptor(), offsets);

  struct PushConstants {
    glm::vec4 color;
    glm::vec4 scale;
    glm::uvec4 index;
  };

  PushConstants pc{};
  pc.color = color;
  pc.scale = glm::vec4(scale);
  pc.index.x = instanceStart;

  commandList.pushConstants(
      pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
      sizeof(PushConstants), &pc);

  commandList.draw(4, 0, instanceEnd - instanceStart, instanceStart);
}

void EditorRenderer::renderMeshOutlines(rhi::RenderCommandList &commandList,
                                        EditorRendererFrameData &frameData,
                                        rhi::PipelineHandle pipeline,
                                        uint32_t instanceStart,
                                        uint32_t instanceEnd, glm::vec4 color,
                                        float scale) {
  std::array<uint32_t, 1> offsets{0};

  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(
      pipeline, 0, frameData.getBindlessParams().getDescriptor(), offsets);

  struct PushConstants {
    glm::vec4 color;
    glm::vec4 scale;
    glm::uvec4 index;
  };

  PushConstants pc{};
  pc.color = color;
  pc.scale = glm::vec4(scale);
  pc.index.x = instanceStart;

  commandList.pushConstants(
      pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
      sizeof(PushConstants), &pc);

  for (uint32_t instanceIndex = instanceStart; instanceIndex < instanceEnd;
       ++instanceIndex) {
    const auto &outline = frameData.getMeshOutlines().at(instanceIndex);
    commandList.bindVertexBuffer(outline.vertexBuffer);
    commandList.bindIndexBuffer(outline.indexBuffer, rhi::IndexType::Uint32);

    for (size_t i = 0; i < outline.indexCounts.size(); ++i) {
      commandList.drawIndexed(
          outline.indexCounts.at(i), outline.indexOffsets.at(i),
          static_cast<int32_t>(outline.vertexOffsets.at(i)), 1, instanceIndex);
    }
  }
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
