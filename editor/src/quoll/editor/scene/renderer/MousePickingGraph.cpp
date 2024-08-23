#include "quoll/core/Base.h"
#include "quoll/renderer/MeshRenderUtils.h"
#include "quoll/renderer/MeshVertexLayout.h"
#include "quoll/renderer/RenderStorage.h"
#include "MousePickingGraph.h"

namespace quoll::editor {

MousePickingGraph::MousePickingGraph(
    const std::array<SceneRendererFrameData, 2> &frameData,
    AssetRegistry &assetRegistry, RenderStorage &renderStorage)
    : mRenderStorage(renderStorage), mFrameData(frameData),
      mAssetRegistry(assetRegistry), mRenderGraph("MousePicking"),
      mBindlessParams{
          BindlessDrawParameters(renderStorage.getDevice()
                                     ->getDeviceInformation()
                                     .getLimits()
                                     .minUniformBufferOffsetAlignment),
          BindlessDrawParameters(renderStorage.getDevice()
                                     ->getDeviceInformation()
                                     .getLimits()
                                     .minUniformBufferOffsetAlignment)} {
  auto *device = mRenderStorage.getDevice();

  mRenderStorage.createShader("mouse-picking.sprite.vertex",
                              {"assets/shaders/mouse-picking-sprite.vert.spv"});
  mRenderStorage.createShader("mouse-picking.mesh.vertex",
                              {"assets/shaders/mouse-picking-mesh.vert.spv"});
  mRenderStorage.createShader(
      "mouse-picking.skinned-mesh.vertex",
      {"assets/shaders/mouse-picking-skinned-mesh.vert.spv"});
  mRenderStorage.createShader("mouse-picking.text.vertex",
                              {"assets/shaders/mouse-picking-text.vert.spv"});
  mRenderStorage.createShader(
      "mouse-picking.selector.fragment",
      {"assets/shaders/mouse-picking-selector.frag.spv"});

  u32 nullEntity = 0;
  mSelectedEntityBuffer = renderStorage.createBuffer(
      {rhi::BufferUsage::Storage, sizeof(u32), &nullEntity,
       rhi::BufferAllocationUsage::HostRead});

  rhi::BufferDescription defaultDesc{};
  defaultDesc.usage = rhi::BufferUsage::Storage;
  defaultDesc.size = sizeof(u32) * mFrameData.at(0).getReservedSpace();
  defaultDesc.mapped = true;

  {
    auto desc = defaultDesc;
    desc.debugName = "sprite entities";
    mSpriteEntitiesBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "mesh entities";
    mMeshEntitiesBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "skinned mesh entities";
    mSkinnedMeshEntitiesBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "text entities";
    mTextEntitiesBuffer = renderStorage.createBuffer(desc);
  }
}

void MousePickingGraph::execute(rhi::RenderCommandList &commandList,
                                const glm::vec2 &mousePos, u32 frameIndex) {
  mFrameIndex = frameIndex;
  const auto &frameData = mFrameData.at(frameIndex);

  auto &textBounds = mMousePickingFrameData.at(frameIndex).textBounds;
  textBounds.clear();
  textBounds.reserve(frameData.getTexts().size());

  for (const auto &text : frameData.getTexts()) {
    glm::vec4 bounds(
        std::numeric_limits<f32>::max(), std::numeric_limits<f32>::max(),
        std::numeric_limits<f32>::min(), std::numeric_limits<f32>::min());

    for (auto i = text.glyphStart; i < text.glyphStart + text.length; ++i) {
      const auto &glyph = frameData.getTextGlyphs().at(static_cast<usize>(i));

      bounds.x = std::min(glyph.planeBounds.x, bounds.x);
      bounds.y = std::min(glyph.planeBounds.y, bounds.y);
      bounds.z = std::max(glyph.planeBounds.z, bounds.z);
      bounds.w = std::max(glyph.planeBounds.w, bounds.w);
    }

    textBounds.push_back(bounds);
  }

  mSpriteEntitiesBuffer.update(frameData.getSpriteEntities().data(),
                               frameData.getSpriteEntities().size() *
                                   sizeof(Entity));
  mTextEntitiesBuffer.update(frameData.getTextEntities().data(),
                             frameData.getTextEntities().size() *
                                 sizeof(Entity));

  {
    usize offset = 0;
    auto *bufferData = static_cast<Entity *>(mMeshEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mMeshEntitiesBuffer.unmap();
  }

  {
    usize offset = 0;
    auto *bufferData = static_cast<Entity *>(mSkinnedMeshEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getSkinnedMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mSkinnedMeshEntitiesBuffer.unmap();
  }

  mMousePos = mousePos;

  recreateIfResized();

  mRenderGraph.execute(commandList, frameIndex);
}

Entity MousePickingGraph::getSelectedEntity() {
  flecs::entity selectedEntity;

  auto *data = mSelectedEntityBuffer.map();
  memcpy(&selectedEntity, data, sizeof(Entity));
  mSelectedEntityBuffer.unmap();

  flecs::entity nullEntity;
  mSelectedEntityBuffer.update(&nullEntity, sizeof(Entity));

  mFrameIndex = std::numeric_limits<u32>::max();

  return selectedEntity;
}

void MousePickingGraph::setFramebufferSize(glm::uvec2 size) {
  mFramebufferSize = size;
  mResized = true;
}

void MousePickingGraph::recreateIfResized() {
  if (!mResized)
    return;

  mRenderGraph.destroy(mRenderStorage);
  mRenderGraph = RenderGraph("Mouse picking");

  createRenderGraph();

  mRenderGraph.build(mRenderStorage);

  mResized = false;
}

void MousePickingGraph::createRenderGraph() {
  for (auto &params : mBindlessParams) {
    params.destroy(mRenderStorage.getDevice());
  }

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = mFramebufferSize.x;
  depthBufferDesc.height = mFramebufferSize.y;
  depthBufferDesc.layerCount = 1;
  depthBufferDesc.samples = 1;
  depthBufferDesc.format = rhi::Format::Depth32Float;
  depthBufferDesc.debugName = "Mouse picking depth stencil";

  auto depthBuffer = mRenderGraph.create(depthBufferDesc);

  auto &pass = mRenderGraph.addGraphicsPass("MousePicking");
  pass.write(depthBuffer, AttachmentType::Depth,
             rhi::DepthStencilClear({1.0f, 0}));

  // Sprites
  auto spritePipeline =
      mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("mouse-picking.sprite.vertex"),
          mRenderStorage.getShader("mouse-picking.selector.fragment"),
          {},
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}},
          {},
          {},
          "mouse picking sprite"});

  // Normal meshes
  auto meshPipeline =
      mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("mouse-picking.mesh.vertex"),
          mRenderStorage.getShader("mouse-picking.selector.fragment"),
          createMeshPositionLayout(),
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}},
          {},
          {},
          "mouse picking mesh"});

  // Skinned meshes
  auto skinnedMeshPipeline =
      mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("mouse-picking.skinned-mesh.vertex"),
          mRenderStorage.getShader("mouse-picking.selector.fragment"),
          createSkinnedMeshPositionLayout(),
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}},
          {},
          {},
          "mouse picking skinned mesh"});

  // Texts
  auto textPipeline =
      mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("mouse-picking.text.vertex"),
          mRenderStorage.getShader("mouse-picking.selector.fragment"),
          {},
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}},
          {},
          {},
          "mouse picking text"});

  pass.addPipeline(spritePipeline);
  pass.addPipeline(meshPipeline);
  pass.addPipeline(skinnedMeshPipeline);
  pass.addPipeline(textPipeline);

  struct MousePickingDrawParams {
    rhi::DeviceAddress selectedEntity;
    rhi::DeviceAddress camera;

    rhi::DeviceAddress spriteTransforms;
    rhi::DeviceAddress spriteEntities;

    rhi::DeviceAddress meshTransforms;
    rhi::DeviceAddress meshEntities;

    rhi::DeviceAddress skinnedMeshTransforms;
    rhi::DeviceAddress skinnedMeshEntities;
    rhi::DeviceAddress skeletons;

    rhi::DeviceAddress textTransforms;
    rhi::DeviceAddress textEntities;
    rhi::DeviceAddress glyphs;
  };

  usize offset = 0;
  for (usize i = 0; i < mBindlessParams.size(); ++i) {
    auto &frameData = mFrameData.at(i);
    offset = mBindlessParams.at(i).addRange(MousePickingDrawParams{
        mSelectedEntityBuffer.getAddress(), frameData.getCameraBuffer(),

        frameData.getSpriteTransformsBuffer(),
        mSpriteEntitiesBuffer.getAddress(),

        frameData.getMeshTransformsBuffer(), mMeshEntitiesBuffer.getAddress(),

        frameData.getSkinnedMeshTransformsBuffer(),
        mSkinnedMeshEntitiesBuffer.getAddress(), frameData.getSkeletonsBuffer(),

        frameData.getTextTransformsBuffer(), mTextEntitiesBuffer.getAddress(),
        frameData.getGlyphsBuffer()});
  }

  pass.setExecutor([this, spritePipeline, meshPipeline, skinnedMeshPipeline,
                    textPipeline, offset](rhi::RenderCommandList &commandList,
                                          u32 frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    commandList.setScissor(glm::ivec2(mMousePos), glm::uvec2(1, 1));

    std::array<u32, 1> offsets{static_cast<u32>(offset)};

    // Sprite
    {
      commandList.bindPipeline(spritePipeline);
      commandList.bindDescriptor(spritePipeline, 0,
                                 mBindlessParams.at(frameIndex).getDescriptor(),
                                 offsets);

      commandList.draw(
          4, 0, static_cast<u32>(frameData.getSpriteEntities().size()), 0);
    }

    // Mesh
    {
      commandList.bindPipeline(meshPipeline);

      commandList.bindDescriptor(meshPipeline, 0,
                                 mBindlessParams.at(frameIndex).getDescriptor(),
                                 offsets);

      u32 instanceStart = 0;
      for (auto &[handle, meshData] : frameData.getMeshGroups()) {
        const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;

        u32 numInstances = static_cast<u32>(meshData.transforms.size());

        commandList.bindVertexBuffers(
            MeshRenderUtils::getGeometryBuffers(mesh),
            MeshRenderUtils::getGeometryBufferOffsets(mesh));
        commandList.bindIndexBuffer(mesh.indexBuffer, rhi::IndexType::Uint32);

        i32 vertexOffset = 0;
        u32 indexOffset = 0;
        for (auto &geometry : mesh.geometries) {
          u32 indexCount = static_cast<u32>(geometry.indices.size());
          i32 vertexCount = static_cast<i32>(geometry.positions.size());

          commandList.drawIndexed(indexCount, indexOffset, vertexOffset,
                                  numInstances, instanceStart);
          vertexOffset += vertexCount;
          indexOffset += indexCount;
        }
        instanceStart += numInstances;
      }
    }

    // Skinned meshes
    {
      commandList.bindPipeline(skinnedMeshPipeline);

      commandList.bindDescriptor(skinnedMeshPipeline, 0,
                                 mBindlessParams.at(frameIndex).getDescriptor(),
                                 offsets);

      u32 instanceStart = 0;
      for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
        const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;

        u32 numInstances = static_cast<u32>(meshData.transforms.size());

        commandList.bindVertexBuffers(
            MeshRenderUtils::getSkinnedGeometryBuffers(mesh),
            MeshRenderUtils::getSkinnedGeometryBufferOffsets(mesh));
        commandList.bindIndexBuffer(mesh.indexBuffer, rhi::IndexType::Uint32);

        i32 vertexOffset = 0;
        u32 indexOffset = 0;
        for (auto &geometry : mesh.geometries) {
          u32 indexCount = static_cast<u32>(geometry.indices.size());
          i32 vertexCount = static_cast<i32>(geometry.positions.size());

          commandList.drawIndexed(indexCount, indexOffset, vertexOffset,
                                  numInstances, instanceStart);
          vertexOffset += vertexCount;
          indexOffset += indexCount;
        }
        instanceStart += numInstances;
      }
    }

    // Text
    {
      commandList.bindPipeline(textPipeline);
      commandList.bindDescriptor(textPipeline, 0,
                                 mBindlessParams.at(frameIndex).getDescriptor(),
                                 offsets);

      static constexpr u32 NumVertices = 4;
      for (usize i = 0; i < frameData.getTextEntities().size(); ++i) {
        commandList.pushConstants(
            textPipeline, rhi::ShaderStage::Vertex, 0, sizeof(glm::uvec4),
            glm::value_ptr(
                mMousePickingFrameData.at(frameIndex).textBounds.at(i)));

        commandList.draw(NumVertices, 0, 1, static_cast<u32>(i));
      }
    }
  });

  for (auto &bp : mBindlessParams) {
    bp.build(mRenderStorage.getDevice());
  }
}

} // namespace quoll::editor
