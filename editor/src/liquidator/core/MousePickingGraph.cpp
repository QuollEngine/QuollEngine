#include "liquid/core/Base.h"
#include "MousePickingGraph.h"

namespace liquidator {

MousePickingGraph::MousePickingGraph(liquid::ShaderLibrary &shaderLibrary,
                                     const liquid::RenderStorage &renderStorage,
                                     liquid::AssetRegistry &assetRegistry,
                                     liquid::rhi::RenderDevice *device)
    : mDevice(device), mRenderStorage(renderStorage),
      mAssetRegistry(assetRegistry), mGraphEvaluator(device) {
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

  mEntitiesBuffer = device->createBuffer(
      {liquid::rhi::BufferType::Storage,
       sizeof(liquid::Entity) * mRenderStorage.getReservedSpace()});

  mSkinnedEntitiesBuffer = device->createBuffer(
      {liquid::rhi::BufferType::Storage,
       sizeof(liquid::Entity) * mRenderStorage.getReservedSpace()});

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
                       const liquid::rhi::RenderGraphRegistry &registry) {
    commandList.setScissor(glm::ivec2(mMousePos), glm::uvec2(1, 1));

    auto pipeline = registry.get(vPipeline);
    auto skinnedPipeline = registry.get(vSkinnedPipeline);

    // Mesh
    commandList.bindPipeline(pipeline);

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mRenderStorage.getActiveCameraBuffer(),
                      liquid::rhi::DescriptorType::UniformBuffer);
      commandList.bindDescriptor(pipeline, 0, descriptor);
    }

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mRenderStorage.getMeshTransformsBuffer(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      descriptor.bind(1, mEntitiesBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(pipeline, 1, descriptor);
    }

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mSelectedEntityBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(pipeline, 2, descriptor);
    }

    for (auto &[handle, meshData] : mRenderStorage.getMeshGroups()) {
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

        for (auto index : meshData.indices) {
          if (indexed) {
            commandList.drawIndexed(indexCount, 0, 0, 1, index);
          } else {
            commandList.draw(vertexCount, 0, 1, index);
          }
        }
      }
    }

    // Skinned meshes
    commandList.bindPipeline(skinnedPipeline);

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mRenderStorage.getActiveCameraBuffer(),
                      liquid::rhi::DescriptorType::UniformBuffer);
      commandList.bindDescriptor(skinnedPipeline, 0, descriptor);
    }

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mRenderStorage.getSkinnedMeshTransformsBuffer(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      descriptor.bind(1, mRenderStorage.getSkeletonsBuffer(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      descriptor.bind(2, mSkinnedEntitiesBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(skinnedPipeline, 1, descriptor);
    }

    {
      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, mSelectedEntityBuffer.getHandle(),
                      liquid::rhi::DescriptorType::StorageBuffer);
      commandList.bindDescriptor(skinnedPipeline, 2, descriptor);
    }

    for (auto &[handle, meshData] : mRenderStorage.getSkinnedMeshGroups()) {
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

        for (auto index : meshData.indices) {
          if (indexed) {
            commandList.drawIndexed(indexCount, 0, 0, 1, index);
          } else {
            commandList.draw(vertexCount, 0, 1, index);
          }
        }
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

  const auto &meshEntities = mRenderStorage.getMeshEntities();
  mEntitiesBuffer.update(const_cast<liquid::Entity *>(meshEntities.data()));

  const auto &skinnedMeshEntities = mRenderStorage.getSkinnedMeshEntities();
  mSkinnedEntitiesBuffer.update(
      const_cast<liquid::Entity *>(skinnedMeshEntities.data()));

  mMousePos = mousePos;

  mGraphEvaluator.execute(commandList, mRenderGraph);
}

liquid::Entity MousePickingGraph::getSelectedEntity() {
  auto selectedEntity = liquid::EntityNull;

  auto *data = mSelectedEntityBuffer.map();
  memcpy(&selectedEntity, data, sizeof(liquid::Entity));
  mSelectedEntityBuffer.unmap();

  liquid::Entity nullEntity = liquid::EntityNull;
  mSelectedEntityBuffer.update(&nullEntity);

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
