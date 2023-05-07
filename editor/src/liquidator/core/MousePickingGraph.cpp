#include "liquid/core/Base.h"
#include "MousePickingGraph.h"

namespace liquid::editor {

MousePickingGraph::MousePickingGraph(
    ShaderLibrary &shaderLibrary,
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

  static constexpr uint32_t FramebufferSizePercentage = 100;

  shaderLibrary.addShader(
      "mouse-picking.default.vertex",
      device->createShader({"assets/shaders/mouse-picking.vert.spv"}));
  shaderLibrary.addShader(
      "mouse-picking.skinned.vertex",
      device->createShader({"assets/shaders/mouse-picking-skinned.vert.spv"}));
  shaderLibrary.addShader(
      "mouse-picking.selector.fragment",
      device->createShader({"assets/shaders/mouse-picking.frag.spv"}));

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = FramebufferSizePercentage;
  depthBufferDesc.height = FramebufferSizePercentage;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = rhi::Format::Depth32Float;
  auto depthBuffer =
      renderStorage.createFramebufferRelativeTexture(depthBufferDesc);

  Entity nullEntity{0};
  mSelectedEntityBuffer = renderStorage.createBuffer(
      {rhi::BufferUsage::Storage, sizeof(Entity), &nullEntity,
       rhi::BufferAllocationUsage::HostRead});

  rhi::BufferDescription defaultDesc{};
  defaultDesc.usage = rhi::BufferUsage::Storage;
  defaultDesc.size = sizeof(Entity) * mFrameData.at(0).getReservedSpace();
  defaultDesc.mapped = true;

  mEntitiesBuffer = renderStorage.createBuffer(defaultDesc);
  mSkinnedEntitiesBuffer = renderStorage.createBuffer(defaultDesc);

  auto &pass = mRenderGraph.addGraphicsPass("MousePicking");
  pass.write(depthBuffer, AttachmentType::Depth,
             rhi::DepthStencilClear({1.0f, 0}));

  // Normal meshes
  auto pipeline = renderStorage.addPipeline(rhi::GraphicsPipelineDescription{
      shaderLibrary.getShader("mouse-picking.default.vertex"),
      shaderLibrary.getShader("mouse-picking.selector.fragment"),
      rhi::PipelineVertexInputLayout::create<Vertex>(),
      rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
      rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                              rhi::FrontFace::CounterClockwise},
      rhi::PipelineColorBlend{{}}});

  // Skinned meshes
  auto skinnedPipeline =
      renderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          shaderLibrary.getShader("mouse-picking.skinned.vertex"),
          shaderLibrary.getShader("mouse-picking.selector.fragment"),
          rhi::PipelineVertexInputLayout::create<Vertex>(),
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                                  rhi::FrontFace::CounterClockwise},
          rhi::PipelineColorBlend{{}}});

  pass.addPipeline(pipeline);
  pass.addPipeline(skinnedPipeline);

  struct MousePickingDrawParams {
    rhi::BufferHandle meshTransforms;
    rhi::BufferHandle skinnedMeshTransforms;
    rhi::BufferHandle skeletons;
    rhi::BufferHandle camera;
    rhi::BufferHandle entities;
    rhi::BufferHandle selectedEntity;
    uint32_t pad0;
    uint32_t pad1;
  };

  size_t offset = 0;
  for (size_t i = 0; i < mBindlessParams.size(); ++i) {
    auto &frameData = mFrameData.at(i);
    offset = mBindlessParams.at(i).addRange(MousePickingDrawParams{
        frameData.getMeshTransformsBuffer(),
        frameData.getSkinnedMeshTransformsBuffer(),
        frameData.getSkeletonsBuffer(), frameData.getCameraBuffer(),
        mEntitiesBuffer.getHandle(), mSelectedEntityBuffer.getHandle()});
  }

  pass.setExecutor([this, pipeline, skinnedPipeline, offset,
                    &renderStorage](rhi::RenderCommandList &commandList,
                                    uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    commandList.setScissor(glm::ivec2(mMousePos), glm::uvec2(1, 1));

    std::array<uint32_t, 1> offsets{static_cast<uint32_t>(offset)};
    // Mesh
    {
      commandList.bindPipeline(pipeline);

      commandList.bindDescriptor(pipeline, 0,
                                 renderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(
          pipeline, 1, mBindlessParams.at(frameIndex).getDescriptor(), offsets);

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
      commandList.bindPipeline(skinnedPipeline);

      commandList.bindDescriptor(skinnedPipeline, 0,
                                 renderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(skinnedPipeline, 1,
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

MousePickingGraph::~MousePickingGraph() {}

void MousePickingGraph::compile() { mRenderGraph.build(mRenderStorage); }

void MousePickingGraph::execute(rhi::RenderCommandList &commandList,
                                const glm::vec2 &mousePos,
                                uint32_t frameIndex) {
  mFrameIndex = frameIndex;
  const auto &frameData = mFrameData.at(frameIndex);

  {
    size_t offset = 0;
    auto *bufferData = static_cast<Entity *>(mEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mEntitiesBuffer.unmap();
  }

  {
    size_t offset = 0;
    auto *bufferData = static_cast<Entity *>(mSkinnedEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getSkinnedMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mSkinnedEntitiesBuffer.unmap();
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
