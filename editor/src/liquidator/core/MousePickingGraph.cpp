#include "liquid/core/Base.h"
#include "MousePickingGraph.h"

namespace liquid::editor {

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
  mRenderStorage.createShader(
      "mouse-picking.selector.fragment",
      {"assets/shaders/mouse-picking-selector.frag.spv"});

  auto depthBuffer = mRenderGraph.create(
      [this](auto width, auto height) {
        rhi::TextureDescription description{};
        description.usage =
            rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
        description.width = width;
        description.height = height;
        description.layers = 1;
        description.samples = 1;
        description.format = rhi::Format::Depth32Float;
        description.debugName = "Mouse picking depth stencil";

        return description;
      },
      [](auto handle, RenderStorage &storage) {});

  Entity nullEntity{0};
  mSelectedEntityBuffer = renderStorage.createBuffer(
      {rhi::BufferUsage::Storage, sizeof(Entity), &nullEntity,
       rhi::BufferAllocationUsage::HostRead});

  rhi::BufferDescription defaultDesc{};
  defaultDesc.usage = rhi::BufferUsage::Storage;
  defaultDesc.size = sizeof(Entity) * mFrameData.at(0).getReservedSpace();
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

  auto &pass = mRenderGraph.addGraphicsPass("MousePicking");
  pass.write(depthBuffer, AttachmentType::Depth,
             rhi::DepthStencilClear({1.0f, 0}));

  // Sprites
  auto spritePipeline =
      renderStorage.addPipeline(rhi::GraphicsPipelineDescription{
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
      renderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("mouse-picking.mesh.vertex"),
          mRenderStorage.getShader("mouse-picking.selector.fragment"),
          rhi::PipelineVertexInputLayout::create<Vertex>(),
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}},
          {},
          {},
          "mouse picking mesh"});

  // Skinned meshes
  auto skinnedMeshPipeline =
      renderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("mouse-picking.skinned-mesh.vertex"),
          mRenderStorage.getShader("mouse-picking.selector.fragment"),
          rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}},
          {},
          {},
          "mouse picking skinned mesh"});

  pass.addPipeline(spritePipeline);
  pass.addPipeline(meshPipeline);
  pass.addPipeline(skinnedMeshPipeline);

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
  };

  size_t offset = 0;
  for (size_t i = 0; i < mBindlessParams.size(); ++i) {
    auto &frameData = mFrameData.at(i);
    offset = mBindlessParams.at(i).addRange(MousePickingDrawParams{
        mSelectedEntityBuffer.getAddress(),
        frameData.getCameraBuffer(),

        frameData.getSpriteTransformsBuffer(),
        mSpriteEntitiesBuffer.getAddress(),

        frameData.getMeshTransformsBuffer(),
        mMeshEntitiesBuffer.getAddress(),

        frameData.getSkinnedMeshTransformsBuffer(),
        mSkinnedMeshEntitiesBuffer.getAddress(),
        frameData.getSkeletonsBuffer(),
    });
  }

  pass.setExecutor([this, spritePipeline, meshPipeline, skinnedMeshPipeline,
                    offset, &renderStorage](rhi::RenderCommandList &commandList,
                                            uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    commandList.setScissor(glm::ivec2(mMousePos), glm::uvec2(1, 1));

    std::array<uint32_t, 1> offsets{static_cast<uint32_t>(offset)};

    // Sprite
    {
      commandList.bindPipeline(spritePipeline);
      commandList.bindDescriptor(spritePipeline, 0,
                                 mBindlessParams.at(frameIndex).getDescriptor(),
                                 offsets);

      commandList.draw(
          4, 0, static_cast<uint32_t>(frameData.getSpriteEntities().size()), 0);
    }

    // Mesh
    {
      commandList.bindPipeline(meshPipeline);

      commandList.bindDescriptor(meshPipeline, 0,
                                 mBindlessParams.at(frameIndex).getDescriptor(),
                                 offsets);

      uint32_t instanceStart = 0;
      for (auto &[handle, meshData] : frameData.getMeshGroups()) {
        const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;

        uint32_t numInstances =
            static_cast<uint32_t>(meshData.transforms.size());
        commandList.bindVertexBuffer(mesh.vertexBuffer.getHandle());
        commandList.bindIndexBuffer(mesh.indexBuffer.getHandle(),
                                    rhi::IndexType::Uint32);

        int32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        for (auto &geometry : mesh.geometries) {
          uint32_t indexCount = static_cast<uint32_t>(geometry.indices.size());
          int32_t vertexCount = static_cast<int32_t>(geometry.vertices.size());

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

      uint32_t instanceStart = 0;
      for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
        const auto &mesh =
            mAssetRegistry.getSkinnedMeshes().getAsset(handle).data;

        uint32_t numInstances =
            static_cast<uint32_t>(meshData.transforms.size());
        commandList.bindVertexBuffer(mesh.vertexBuffer.getHandle());
        commandList.bindIndexBuffer(mesh.indexBuffer.getHandle(),
                                    rhi::IndexType::Uint32);

        int32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        for (auto &geometry : mesh.geometries) {
          uint32_t indexCount = static_cast<uint32_t>(geometry.indices.size());
          int32_t vertexCount = static_cast<int32_t>(geometry.vertices.size());

          commandList.drawIndexed(indexCount, indexOffset, vertexOffset,
                                  numInstances, instanceStart);
          vertexOffset += vertexCount;
          indexOffset += indexCount;
        }
        instanceStart += numInstances;
      }
    }
  });

  for (auto &bp : mBindlessParams) {
    bp.build(device);
  }
}

void MousePickingGraph::compile() { mRenderGraph.build(mRenderStorage); }

void MousePickingGraph::execute(rhi::RenderCommandList &commandList,
                                const glm::vec2 &mousePos,
                                uint32_t frameIndex) {
  mFrameIndex = frameIndex;
  const auto &frameData = mFrameData.at(frameIndex);

  mSpriteEntitiesBuffer.update(frameData.getSpriteEntities().data(),
                               frameData.getSpriteEntities().size() *
                                   sizeof(Entity));

  {
    size_t offset = 0;
    auto *bufferData = static_cast<Entity *>(mMeshEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mMeshEntitiesBuffer.unmap();
  }

  {
    size_t offset = 0;
    auto *bufferData = static_cast<Entity *>(mSkinnedMeshEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getSkinnedMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mSkinnedMeshEntitiesBuffer.unmap();
  }

  mMousePos = mousePos;

  mRenderGraph.execute(commandList, frameIndex);
}

Entity MousePickingGraph::getSelectedEntity() {
  auto selectedEntity = Entity::Null;

  auto *data = mSelectedEntityBuffer.map();
  memcpy(&selectedEntity, data, sizeof(Entity));
  mSelectedEntityBuffer.unmap();

  Entity nullEntity = Entity::Null;
  mSelectedEntityBuffer.update(&nullEntity, sizeof(Entity));

  mFrameIndex = std::numeric_limits<uint32_t>::max();

  return selectedEntity;
}

void MousePickingGraph::setFramebufferSize(Window &window) {
  mRenderGraph.setFramebufferExtent(window.getFramebufferSize());
  window.addResizeHandler([this](auto width, auto height) {
    mRenderGraph.setFramebufferExtent({width, height});
  });
}

} // namespace liquid::editor
