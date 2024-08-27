#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Profiler.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/MeshVertexLayout.h"
#include "quoll/scene/Sprite.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/text/Text.h"
#include "quoll/editor/ui/IconRegistry.h"
#include "EditorRenderer.h"

namespace quoll::editor {

EditorRenderer::EditorRenderer(RenderStorage &renderStorage,
                               rhi::RenderDevice *device)
    : mRenderStorage(renderStorage),
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
  mRenderStorage.createShader("outline-text.vert",
                              {shadersPath / "outline-text.vert.spv"});
  mRenderStorage.createShader("outline-text.frag",
                              {shadersPath / "outline-text.frag.spv"});

  {
    rhi::SamplerDescription description{};
    description.wrapModeU = rhi::WrapMode::ClampToEdge;
    description.wrapModeV = rhi::WrapMode::ClampToEdge;
    description.wrapModeW = rhi::WrapMode::ClampToEdge;
    mTextOutlineSampler = mRenderStorage.createSampler(description);
  }
}

void EditorRenderer::attach(RenderGraph &graph,
                            const SceneRenderPassData &scenePassData,
                            const RendererOptions &options) {
  for (auto &frameData : mFrameData) {
    frameData.getBindlessParams().destroy(mRenderStorage.getDevice());
    frameData.createBindlessParamsRange();
    frameData.getBindlessParams().build(mRenderStorage.getDevice());
  }

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

    static const f32 WireframeLineHeight = 3.0f;

    auto collidableShapePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("collidable-shape.vert"),
         mRenderStorage.getShader("collidable-shape.frag"),
         createMeshPositionLayout(),
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
                                       u32 frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      std::array<u32, 1> offsets{0};
      // Collidable shapes
      if (frameData.isCollidableEntitySelected() &&
          frameData.getCollidableShapeType() != PhysicsGeometryType::Plane) {
        QUOLL_PROFILE_EVENT("EditorPass::CollidableShapes");

        commandList.bindPipeline(collidableShapePipeline);
        commandList.bindDescriptor(
            collidableShapePipeline, 0,
            frameData.getBindlessParams().getDescriptor(), offsets);

        auto type = frameData.getCollidableShapeType();

        std::array<u64, 1> offsets{0};

        if (type == PhysicsGeometryType::Box) {
          commandList.bindVertexBuffers(
              std::array{mCollidableCube.buffer.getHandle()}, offsets);
          commandList.draw(mCollidableCube.vertexCount, 0);
        } else if (type == PhysicsGeometryType::Sphere) {
          commandList.bindVertexBuffers(
              std::array{mCollidableSphere.buffer.getHandle()}, offsets);
          commandList.draw(mCollidableSphere.vertexCount, 0);
        } else if (type == PhysicsGeometryType::Capsule) {
          commandList.bindVertexBuffers(
              std::array{mCollidableCapsule.buffer.getHandle()}, offsets);
          commandList.draw(mCollidableCapsule.vertexCount, 0);
        }
      }

      // Editor grid
      {
        QUOLL_PROFILE_EVENT("EditorPass::EditorGrid");

        commandList.bindPipeline(editorGridPipeline);
        commandList.bindDescriptor(
            editorGridPipeline, 0,
            frameData.getBindlessParams().getDescriptor(), offsets);

        static constexpr u32 GridPlaneNumVertices = 6;
        commandList.draw(GridPlaneNumVertices, 0);
      }

      // Skeleton bones
      if (!frameData.getBoneCounts().empty()) {
        QUOLL_PROFILE_EVENT("EditorPass::SkeletonBones");

        commandList.bindPipeline(skeletonLinesPipeline);
        commandList.bindDescriptor(
            skeletonLinesPipeline, 0,
            frameData.getBindlessParams().getDescriptor(), offsets);

        const auto &numBones = frameData.getBoneCounts();

        for (usize i = 0; i < numBones.size(); ++i) {
          commandList.draw(numBones.at(i), 0, 1, static_cast<u32>(i));
        }
      }

      // Object gizmos
      {
        QUOLL_PROFILE_EVENT("EditorPass::ObjectGizmos");

        commandList.bindPipeline(objectIconsPipeline);
        commandList.bindDescriptor(
            objectIconsPipeline, 0,
            mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindDescriptor(
            objectIconsPipeline, 1,
            frameData.getBindlessParams().getDescriptor(), offsets);

        u32 previousInstance = 0;
        for (auto &[icon, count] : frameData.getGizmoCounts()) {
          glm::uvec4 uIcon{
              rhi::castHandleToUint(icon),
              rhi::castHandleToUint(mRenderStorage.getDefaultSampler()), 0, 0};
          commandList.pushConstants(objectIconsPipeline,
                                    rhi::ShaderStage::Fragment, 0, sizeof(u32),
                                    glm::value_ptr(uIcon));

          commandList.draw(4, 0, count, previousInstance);

          previousInstance += count;
        }
      }
    });
  }

  // outlines
  {
    rhi::TextureDescription depthBufferDesc{};
    depthBufferDesc.usage = rhi::TextureUsage::Depth |
                            rhi::TextureUsage::Stencil |
                            rhi::TextureUsage::Sampled;
    depthBufferDesc.width = options.framebufferSize.x;
    depthBufferDesc.height = options.framebufferSize.y;
    depthBufferDesc.layerCount = 1;
    depthBufferDesc.samples = scenePassData.sampleCount;
    depthBufferDesc.format = rhi::Format::Depth32FloatStencil8Uint;
    depthBufferDesc.debugName = "Editor depth stencil for outline";
    auto depthBuffer = graph.create(depthBufferDesc);

    auto &outlinePass = graph.addGraphicsPass("outlinePass");
    outlinePass.write(scenePassData.sceneColor, AttachmentType::Color,
                      glm::vec4(0.0));
    outlinePass.write(depthBuffer, AttachmentType::Depth,
                      rhi::DepthStencilClear{1.0f, 0});
    outlinePass.write(scenePassData.sceneColorResolved, AttachmentType::Resolve,
                      glm::vec4(0.0));

    static constexpr u32 MaskAll = 0xFF;
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
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add}}},
         outlineWritePipelineStencil,
         {},
         "sprite outline stencil write"});

    auto outlineSpritePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-sprite.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add}}},
         outlinePipelineStencil,
         {},
         "sprite outline"});

    auto outlineTextStencilWritePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-text.vert"),
         mRenderStorage.getShader("outline-text.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add}}},
         outlineWritePipelineStencil,
         {},
         "text outline stencil write"});

    auto outlineTextPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-text.vert"),
         mRenderStorage.getShader("outline-text.frag"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add}}},
         outlinePipelineStencil,
         {},
         "text outline"});

    auto outlineGeometryStencilWritePipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-geometry.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         createMeshPositionLayout(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
             rhi::BlendOp::Add}}},
         outlineWritePipelineStencil,
         {},
         "geometry outline stencil write"});

    auto outlineGeometryPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-geometry.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         createMeshPositionLayout(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
             true, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add, rhi::BlendFactor::One, rhi::BlendFactor::Zero,
             rhi::BlendOp::Add}}},
         outlinePipelineStencil,
         {},
         "geometry outline"});

    auto outlineSkinnedGeometryStencilWritePipeline =
        mRenderStorage.addPipeline(
            {mRenderStorage.getShader("outline-skinned-geometry.vert"),
             mRenderStorage.getShader("outline-color.frag"),
             createSkinnedMeshPositionLayout(),
             rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
             rhi::PipelineRasterizer{rhi::PolygonMode::Fill,
                                     rhi::CullMode::None,
                                     rhi::FrontFace::Clockwise},
             rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
                 true, rhi::BlendFactor::Zero, rhi::BlendFactor::One,
                 rhi::BlendOp::Add, rhi::BlendFactor::Zero,
                 rhi::BlendFactor::DstAlpha, rhi::BlendOp::Add}}},
             outlineWritePipelineStencil,
             {},
             "skinned geometry outline stencil write"});

    auto outlineSkinnedGeometryPipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("outline-skinned-geometry.vert"),
         mRenderStorage.getShader("outline-color.frag"),
         createSkinnedMeshPositionLayout(),
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
         outlinePipelineStencil,
         {},
         "skinned geometry outline"});

    outlinePass.addPipeline(outlineSpriteStencilWritePipeline);
    outlinePass.addPipeline(outlineSpritePipeline);
    outlinePass.addPipeline(outlineTextStencilWritePipeline);
    outlinePass.addPipeline(outlineTextPipeline);
    outlinePass.addPipeline(outlineGeometryStencilWritePipeline);
    outlinePass.addPipeline(outlineGeometryPipeline);
    outlinePass.addPipeline(outlineSkinnedGeometryStencilWritePipeline);
    outlinePass.addPipeline(outlineSkinnedGeometryPipeline);

    outlinePass.setExecutor(
        [outlineSpriteStencilWritePipeline, outlineSpritePipeline,
         outlineTextStencilWritePipeline, outlineTextPipeline,
         outlineGeometryStencilWritePipeline, outlineGeometryPipeline,
         outlineSkinnedGeometryStencilWritePipeline,
         outlineSkinnedGeometryPipeline,
         this](rhi::RenderCommandList &commandList, u32 frameIndex) {
          static constexpr glm::vec4 OutlineColor{1.0f, 0.26f, 0.0f, 1.0f};
          static constexpr f32 OutlineScale = 1.02f;
          static constexpr f32 OutlineScale2D = 1.06f;

          auto &frameData = mFrameData.at(frameIndex);

          auto spriteEnd = static_cast<u32>(frameData.getOutlineSpriteEnd());
          auto textEnd = static_cast<u32>(frameData.getOutlineTextEnd());
          auto meshEnd = static_cast<u32>(frameData.getOutlineMeshEnd());
          auto skinnedMeshEnd =
              static_cast<u32>(frameData.getOutlineSkinnedMeshEnd());

          renderSpriteOutlines(commandList, frameData,
                               outlineSpriteStencilWritePipeline, 0, spriteEnd,
                               glm::vec4{0.0f}, 1.0f);
          renderSpriteOutlines(commandList, frameData, outlineSpritePipeline, 0,
                               spriteEnd, OutlineColor, OutlineScale2D);

          renderTextOutlines(commandList, frameData,
                             outlineTextStencilWritePipeline, spriteEnd,
                             textEnd, glm::vec4{0.0f}, 1.0f);
          renderTextOutlines(commandList, frameData, outlineTextPipeline,
                             spriteEnd, textEnd, OutlineColor, OutlineScale2D);

          renderMeshOutlines(commandList, frameData,
                             outlineGeometryStencilWritePipeline, textEnd,
                             meshEnd, glm::vec4{0.0f}, 1.0f);
          renderMeshOutlines(commandList, frameData, outlineGeometryPipeline,
                             textEnd, meshEnd, OutlineColor, OutlineScale);

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
                                     AssetRegistry &assetRegistry,
                                     u32 frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  QUOLL_PROFILE_EVENT("EditorRenderer::update");
  frameData.clear();

  if (entityDatabase.exists(state.selectedEntity)) {
    if (entityDatabase.has<Sprite>(state.selectedEntity)) {
      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);
      frameData.addSpriteOutline(world.worldTransform);
    } else if (entityDatabase.has<Mesh>(state.selectedEntity) &&
               entityDatabase.has<MeshRenderer>(state.selectedEntity)) {
      auto handle = entityDatabase.get<Mesh>(state.selectedEntity).handle;

      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);

      const auto &data = assetRegistry.get(handle);
      frameData.addMeshOutline(data, world.worldTransform);
    } else if (entityDatabase.has<Mesh>(state.selectedEntity) &&
               entityDatabase.has<SkinnedMeshRenderer>(state.selectedEntity) &&
               entityDatabase.has<Skeleton>(state.selectedEntity)) {
      auto handle = entityDatabase.get<Mesh>(state.selectedEntity).handle;

      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);
      const auto &data = assetRegistry.get(handle);

      const auto &skeleton = entityDatabase.get<Skeleton>(state.selectedEntity)
                                 .jointFinalTransforms;

      frameData.addSkinnedMeshOutline(data, skeleton, world.worldTransform);
    } else if (entityDatabase.has<Text>(state.selectedEntity)) {
      const auto &text = entityDatabase.get<Text>(state.selectedEntity);
      const auto &font = assetRegistry.get(text.font);
      const auto &world =
          entityDatabase.get<WorldTransform>(state.selectedEntity);

      std::vector<SceneRendererFrameData::GlyphData> glyphs(
          text.content.length());
      f32 advanceX = 0;
      f32 advanceY = 0;
      for (usize i = 0; i < text.content.length(); ++i) {
        char c = text.content.at(i);

        if (c == '\n') {
          advanceX = 0.0f;
          advanceY += text.lineHeight * font.fontScale;
          continue;
        }

        const auto &fontGlyph = font.glyphs.at(c);
        glyphs.at(i).atlasBounds = fontGlyph.atlasBounds;
        glyphs.at(i).planeBounds = fontGlyph.planeBounds;

        glyphs.at(i).planeBounds.x += advanceX;
        glyphs.at(i).planeBounds.z += advanceX;
        glyphs.at(i).planeBounds.y -= advanceY;
        glyphs.at(i).planeBounds.w -= advanceY;

        advanceX += fontGlyph.advanceX;
      }

      frameData.addTextOutline(font.deviceHandle, glyphs, world.worldTransform);
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
    frameData.addGizmo(IconRegistry::getIcon(EditorIcon::Sun),
                       world.worldTransform);
  }

  for (auto [entity, world, light] :
       entityDatabase.view<WorldTransform, PointLight>()) {
    frameData.addGizmo(IconRegistry::getIcon(EditorIcon::Light),
                       world.worldTransform);
  }

  for (auto [entity, world, camera] :
       entityDatabase.view<WorldTransform, PerspectiveLens>()) {
    static constexpr f32 NinetyDegreesInRadians = glm::pi<f32>() / 2.0f;

    frameData.addGizmo(IconRegistry::getIcon(EditorIcon::Camera),
                       glm::rotate(world.worldTransform, NinetyDegreesInRadians,
                                   glm::vec3(0, 1, 0)));
  }

  frameData.updateBuffers();
}

void EditorRenderer::renderSpriteOutlines(rhi::RenderCommandList &commandList,
                                          EditorRendererFrameData &frameData,
                                          rhi::PipelineHandle pipeline,
                                          u32 instanceStart, u32 instanceEnd,
                                          glm::vec4 color, f32 scale) {
  std::array<u32, 1> offsets{0};

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

void EditorRenderer::renderTextOutlines(rhi::RenderCommandList &commandList,
                                        EditorRendererFrameData &frameData,
                                        rhi::PipelineHandle pipeline,
                                        u32 instanceStart, u32 instanceEnd,
                                        glm::vec4 color, f32 scale) {
  std::array<u32, 1> offsets{0};

  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(
      pipeline, 0, frameData.getBindlessParams().getDescriptor(), offsets);
  commandList.bindDescriptor(pipeline, 1,
                             mRenderStorage.getGlobalTexturesDescriptor());

  struct PushConstants {
    glm::vec4 color;
    glm::vec4 scale;
    glm::uvec4 index;
  };

  static constexpr u32 QuadNumVertices = 6;
  for (usize i = 0; i < frameData.getTextOutlines().size(); ++i) {
    const auto &text = frameData.getTextOutlines().at(i);

    PushConstants pc{};
    pc.color = color;
    pc.scale = glm::vec4(scale);
    pc.index.x = instanceStart;
    pc.index.y = text.glyphStart;
    pc.index.z = rhi::castHandleToUint(text.fontTexture);
    pc.index.w = rhi::castHandleToUint(mTextOutlineSampler);

    commandList.pushConstants(
        pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
        sizeof(PushConstants), &pc);

    commandList.draw(QuadNumVertices * static_cast<u32>(text.length), 0, 1,
                     static_cast<u32>(i));
  }
}

void EditorRenderer::renderMeshOutlines(rhi::RenderCommandList &commandList,
                                        EditorRendererFrameData &frameData,
                                        rhi::PipelineHandle pipeline,
                                        u32 instanceStart, u32 instanceEnd,
                                        glm::vec4 color, f32 scale) {
  std::array<u32, 1> offsets{0};

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

  for (u32 instanceIndex = instanceStart; instanceIndex < instanceEnd;
       ++instanceIndex) {
    const auto &outline = frameData.getMeshOutlines().at(instanceIndex);

    commandList.bindVertexBuffers(outline.vertexBuffers,
                                  outline.vertexBufferOffsets);
    commandList.bindIndexBuffer(outline.indexBuffer, rhi::IndexType::Uint32);

    for (usize i = 0; i < outline.indexCounts.size(); ++i) {
      commandList.drawIndexed(
          outline.indexCounts.at(i), outline.indexOffsets.at(i),
          static_cast<i32>(outline.vertexOffsets.at(i)), 1, instanceIndex);
    }
  }
}

void EditorRenderer::createCollidableShapes() {
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  static constexpr f32 Pi = glm::pi<f32>();

  using V = glm::vec3;

  // Box shape
  {
    std::vector<glm::vec3> CollidableBoxVertices{
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
         CollidableBoxVertices.size() * sizeof(glm::vec3),
         static_cast<const void *>(CollidableBoxVertices.data())});
    mCollidableCube.vertexCount =
        static_cast<u32>(CollidableBoxVertices.size());
  }

  using CalculationFn = std::function<f32(f32)>;

  static constexpr f32 Radius = 1.0f;

  auto cSinCenter = [](f32 center) {
    return [center](f32 angle) { return Radius * sin(angle) + center; };
  };
  auto cCosCenter = [](f32 center) {
    return [center](f32 angle) { return Radius * cos(angle) + center; };
  };

  auto cSin = cSinCenter(0.0f);
  auto cCos = cCosCenter(0.0f);
  auto cZero = [](f32 angle) { return 0.0f; };

  // Sphere shape
  {
    auto drawUnitCircle = [](std::vector<glm::vec3> &vertices, u32 numSegments,
                             CalculationFn cX, CalculationFn cY,
                             CalculationFn cZ) {
      const f32 SegmentDelta = 2.0f * Pi / static_cast<f32>(numSegments);
      f32 segmentAngle = SegmentDelta;

      V start{cX(0.0f), cY(0.0f), cZ(0.0f)};

      vertices.push_back(start);

      for (u32 i = 0; i < numSegments; ++i) {
        V vertex{cX(segmentAngle), cY(segmentAngle), cZ(segmentAngle)};

        vertices.push_back(vertex);
        vertices.push_back(vertex);

        segmentAngle += SegmentDelta;
      }

      vertices.push_back(start);
    };

    static constexpr u32 NumSegments = 12;
    std::vector<glm::vec3> CollidableSphereVertices;

    drawUnitCircle(CollidableSphereVertices, NumSegments, cZero, cSin, cCos);
    drawUnitCircle(CollidableSphereVertices, NumSegments, cSin, cCos, cZero);

    mCollidableSphere.buffer = mRenderStorage.createBuffer(
        {rhi::BufferUsage::Vertex,
         CollidableSphereVertices.size() * sizeof(glm::vec3),
         static_cast<const void *>(CollidableSphereVertices.data())});
    mCollidableSphere.vertexCount =
        static_cast<u32>(CollidableSphereVertices.size());
  }

  // Capsule shape
  {
    auto drawUnitHalfCircle =
        [](std::vector<glm::vec3> &vertices, u32 numSegments, CalculationFn cX,
           CalculationFn cY, CalculationFn cZ, f32 circleAngle) {
          const f32 SegmentDelta = circleAngle / static_cast<f32>(numSegments);
          f32 segmentAngle = SegmentDelta;

          V start{cX(0.0f), cY(0.0f), cZ(0.0f)};

          vertices.push_back(start);

          for (u32 i = 0; i < numSegments; ++i) {
            V v{cX(segmentAngle), cY(segmentAngle), cZ(segmentAngle)};

            vertices.push_back(v);
            vertices.push_back(v);

            segmentAngle += SegmentDelta;
          }

          vertices.push_back(vertices.at(vertices.size() - 1));
        };

    static constexpr u32 NumSegments = 12;
    std::vector<glm::vec3> CollidableCapsuleVertices;

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
         CollidableCapsuleVertices.size() * sizeof(glm::vec3),
         static_cast<const void *>(CollidableCapsuleVertices.data())});
    mCollidableCapsule.vertexCount =
        static_cast<u32>(CollidableCapsuleVertices.size());
  }

  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

} // namespace quoll::editor
