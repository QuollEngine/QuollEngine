#include "liquid/core/Base.h"
#include "MousePickingGraph.h"

namespace liquidator {

MousePickingGraph::MousePickingGraph(
    liquid::ShaderLibrary &shaderLibrary,
    const std::array<liquid::SceneRendererFrameData, 2> &frameData,
    liquid::AssetRegistry &assetRegistry, liquid::rhi::RenderDevice *device)
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

  liquid::rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = liquid::rhi::TextureSizeMethod::FramebufferRatio;
  depthBufferDesc.usage =
      liquid::rhi::TextureUsage::Depth | liquid::rhi::TextureUsage::Sampled;
  depthBufferDesc.width = FramebufferSizePercentage;
  depthBufferDesc.height = FramebufferSizePercentage;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = liquid::rhi::Format::Depth32Float;
  auto depthBuffer = mDevice->createTexture(depthBufferDesc);

  liquid::Entity nullEntity{0};
  mSelectedEntityBuffer = device->createBuffer(
      {liquid::rhi::BufferType::Storage, sizeof(liquid::Entity), &nullEntity,
       liquid::rhi::BufferUsage::HostRead});

  liquid::rhi::BufferDescription defaultDesc{};
  defaultDesc.type = liquid::rhi::BufferType::Storage;
  defaultDesc.size =
      sizeof(liquid::Entity) * mFrameData.at(0).getReservedSpace();
  defaultDesc.mapped = true;

  mEntitiesBuffer = device->createBuffer(defaultDesc);
  mSkinnedEntitiesBuffer = device->createBuffer(defaultDesc);

  auto &pass = mRenderGraph.addPass("MousePicking");
  pass.write(depthBuffer, liquid::rhi::DepthStencilClear({1.0f, 0}));

  // Normal meshes
  auto vPipeline = pass.addPipeline(liquid::rhi::PipelineDescription{
      shaderLibrary.getShader("mouse-picking.default.vertex"),
      shaderLibrary.getShader("mouse-picking.selector.fragment"),
      liquid::rhi::PipelineVertexInputLayout::create<liquid::Vertex>(),
      liquid::rhi::PipelineInputAssembly{
          liquid::rhi::PrimitiveTopology::TriangleList},
      liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                      liquid::rhi::CullMode::Back,
                                      liquid::rhi::FrontFace::CounterClockwise},
      liquid::rhi::PipelineColorBlend{{}}});

  // Skinned meshes
  auto vSkinnedPipeline = pass.addPipeline(liquid::rhi::PipelineDescription{
      shaderLibrary.getShader("mouse-picking.skinned.vertex"),
      shaderLibrary.getShader("mouse-picking.selector.fragment"),
      liquid::rhi::PipelineVertexInputLayout::create<liquid::Vertex>(),
      liquid::rhi::PipelineInputAssembly{
          liquid::rhi::PrimitiveTopology::TriangleList},
      liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                      liquid::rhi::CullMode::Back,
                                      liquid::rhi::FrontFace::CounterClockwise},
      liquid::rhi::PipelineColorBlend{{}}});

  pass.setExecutor([this, vPipeline, vSkinnedPipeline](
                       liquid::rhi::RenderCommandList &commandList,
                       const liquid::RenderGraphRegistry &registry,
                       uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    commandList.setScissor(glm::ivec2(mMousePos), glm::uvec2(1, 1));

    auto pipeline = registry.get(vPipeline);
    auto skinnedPipeline = registry.get(vSkinnedPipeline);

    // Mesh
    commandList.bindPipeline(pipeline);

    commandList.bindDescriptor(pipeline, 0, frameData.getGlobalDescriptor());
    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mEntitiesBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(pipeline, 1, descriptor);
    }

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mSelectedEntityBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(pipeline, 2, descriptor);
    }

    uint32_t instanceStart = 0;
    for (auto &[handle, meshData] : frameData.getMeshGroups()) {
      const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;
      for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
        commandList.bindVertexBuffer(mesh.vertexBuffers.at(g).getHandle());
        bool indexed =
            liquid::rhi::isHandleValid(mesh.indexBuffers.at(g).getHandle());

        if (indexed) {
          commandList.bindIndexBuffer(mesh.indexBuffers.at(g).getHandle(),
                                      liquid::rhi::IndexType::Uint32);
        }

        uint32_t indexCount =
            static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
        uint32_t vertexCount =
            static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

        if (indexed) {
          commandList.drawIndexed(
              indexCount, 0, 0,
              static_cast<uint32_t>(meshData.transforms.size()), instanceStart);
        } else {
          commandList.draw(vertexCount, 0,
                           static_cast<uint32_t>(meshData.transforms.size()),
                           instanceStart);
        }

        instanceStart += static_cast<uint32_t>(meshData.transforms.size());
      }
    }

    // Skinned meshes
    commandList.bindPipeline(skinnedPipeline);

    instanceStart = 0;

    commandList.bindDescriptor(pipeline, 0, frameData.getGlobalDescriptor());

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mSkinnedEntitiesBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(skinnedPipeline, 1, descriptor);
    }

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mSelectedEntityBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(skinnedPipeline, 2, descriptor);
    }

    for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
      const auto &mesh =
          mAssetRegistry.getSkinnedMeshes().getAsset(handle).data;
      for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
        commandList.bindVertexBuffer(mesh.vertexBuffers.at(g).getHandle());
        bool indexed =
            liquid::rhi::isHandleValid(mesh.indexBuffers.at(g).getHandle());
        if (indexed) {
          commandList.bindIndexBuffer(mesh.indexBuffers.at(g).getHandle(),
                                      liquid::rhi::IndexType::Uint32);
        }

        uint32_t indexCount =
            static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
        uint32_t vertexCount =
            static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

        if (indexed) {
          commandList.drawIndexed(
              indexCount, 0, 0,
              static_cast<uint32_t>(meshData.transforms.size()), instanceStart);
        } else {
          commandList.draw(vertexCount, 0,
                           static_cast<uint32_t>(meshData.transforms.size()),
                           instanceStart);
        }

        instanceStart += static_cast<uint32_t>(meshData.transforms.size());
      }
    }
  });
}

MousePickingGraph::~MousePickingGraph() {}

void MousePickingGraph::compile() {
  mRenderGraph.compile(mDevice);
  mGraphEvaluator.build(mRenderGraph);
}

void MousePickingGraph::execute(liquid::rhi::RenderCommandList &commandList,
                                const glm::vec2 &mousePos,
                                uint32_t frameIndex) {
  mFrameIndex = frameIndex;
  auto &frameData = mFrameData.at(frameIndex);

  {
    size_t offset = 0;
    auto *bufferData = static_cast<liquid::Entity *>(mEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(liquid::Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mEntitiesBuffer.unmap();
  }

  {
    size_t offset = 0;
    auto *bufferData =
        static_cast<liquid::Entity *>(mSkinnedEntitiesBuffer.map());
    for (auto &[_, meshData] : frameData.getSkinnedMeshGroups()) {
      memcpy(bufferData + offset, meshData.entities.data(),
             sizeof(liquid::Entity) * meshData.entities.size());
      offset += meshData.entities.size();
    }
    mSkinnedEntitiesBuffer.unmap();
  }

  mMousePos = mousePos;

  mGraphEvaluator.execute(commandList, mRenderGraph, frameIndex);
}

liquid::Entity MousePickingGraph::getSelectedEntity() {
  auto selectedEntity = liquid::EntityNull;

  auto *data = mSelectedEntityBuffer.map();
  memcpy(&selectedEntity, data, sizeof(liquid::Entity));
  mSelectedEntityBuffer.unmap();

  liquid::Entity nullEntity = liquid::EntityNull;
  mSelectedEntityBuffer.update(&nullEntity, sizeof(liquid::Entity));

  mFrameIndex = std::numeric_limits<uint32_t>::max();

  return selectedEntity;
}

void MousePickingGraph::setFramebufferSize(liquid::Window &window) {
  mRenderGraph.setFramebufferExtent(window.getFramebufferSize());
  window.addResizeHandler([this](auto width, auto height) {
    mRenderGraph.setFramebufferExtent({width, height});
  });
}

} // namespace liquidator
