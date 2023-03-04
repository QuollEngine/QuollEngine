#include "liquid/core/Base.h"
#include "MousePickingGraph.h"

namespace liquid::editor {

MousePickingGraph::MousePickingGraph(
    ShaderLibrary &shaderLibrary,
    const std::array<SceneRendererFrameData, 2> &frameData,
    AssetRegistry &assetRegistry, RenderStorage &renderStorage,
    rhi::RenderDevice *device)
    : mDevice(device), mFrameData(frameData), mAssetRegistry(assetRegistry),
      mGraphEvaluator(device), mRenderGraph("MousePicking") {
  static constexpr uint32_t FramebufferSizePercentage = 100;

  shaderLibrary.addShader(
      "mouse-picking.default.vertex",
      mDevice->createShader({"assets/shaders/mouse-picking.vert.spv"}));
  shaderLibrary.addShader(
      "mouse-picking.skinned.vertex",
      mDevice->createShader({"assets/shaders/mouse-picking-skinned.vert.spv"}));
  shaderLibrary.addShader(
      "mouse-picking.selector.fragment",
      mDevice->createShader({"assets/shaders/mouse-picking.frag.spv"}));

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = FramebufferSizePercentage;
  depthBufferDesc.height = FramebufferSizePercentage;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = rhi::Format::Depth32Float;
  auto depthBuffer = renderStorage.createTexture(depthBufferDesc);

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
  pass.write(depthBuffer, rhi::DepthStencilClear({1.0f, 0}));

  // Normal meshes
  auto vPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
      shaderLibrary.getShader("mouse-picking.default.vertex"),
      shaderLibrary.getShader("mouse-picking.selector.fragment"),
      rhi::PipelineVertexInputLayout::create<Vertex>(),
      rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
      rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                              rhi::FrontFace::CounterClockwise},
      rhi::PipelineColorBlend{{}}});

  // Skinned meshes
  auto vSkinnedPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
      shaderLibrary.getShader("mouse-picking.skinned.vertex"),
      shaderLibrary.getShader("mouse-picking.selector.fragment"),
      rhi::PipelineVertexInputLayout::create<Vertex>(),
      rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
      rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Back,
                              rhi::FrontFace::CounterClockwise},
      rhi::PipelineColorBlend{{}}});

  pass.setExecutor([this, vPipeline, vSkinnedPipeline,
                    &renderStorage](rhi::RenderCommandList &commandList,
                                    const RenderGraphRegistry &registry,
                                    uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    auto drawParams = frameData.getDrawParams();
    drawParams.index9 =
        rhi::castHandleToUint(mSelectedEntityBuffer.getHandle());
    drawParams.index10 = rhi::castHandleToUint(mEntitiesBuffer.getHandle());

    commandList.setScissor(glm::ivec2(mMousePos), glm::uvec2(1, 1));

    auto pipeline = registry.get(vPipeline);
    auto skinnedPipeline = registry.get(vSkinnedPipeline);

    // Mesh
    {
      commandList.bindPipeline(pipeline);

      commandList.bindDescriptor(pipeline, 0,
                                 renderStorage.getGlobalBuffersDescriptor());
      commandList.pushConstants(skinnedPipeline,
                                rhi::ShaderStage::Vertex |
                                    rhi::ShaderStage::Fragment,
                                0, sizeof(DrawParameters), &drawParams);

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
      auto drawParams = frameData.getDrawParams();
      drawParams.index9 =
          rhi::castHandleToUint(mSelectedEntityBuffer.getHandle());
      drawParams.index10 = rhi::castHandleToUint(mEntitiesBuffer.getHandle());

      commandList.bindPipeline(skinnedPipeline);

      commandList.bindDescriptor(skinnedPipeline, 0,
                                 renderStorage.getGlobalBuffersDescriptor());

      commandList.pushConstants(skinnedPipeline,
                                rhi::ShaderStage::Vertex |
                                    rhi::ShaderStage::Fragment,
                                0, sizeof(DrawParameters), &drawParams);

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
}

MousePickingGraph::~MousePickingGraph() {}

void MousePickingGraph::compile() {
  mRenderGraph.compile(mDevice);
  mGraphEvaluator.build(mRenderGraph);
}

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

  mGraphEvaluator.execute(commandList, mRenderGraph, frameIndex);
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
