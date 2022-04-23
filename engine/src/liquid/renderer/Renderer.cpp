#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "liquid/renderer/SceneRenderer.h"

#include "liquid/rhi/RenderGraph.h"

namespace liquid {

Renderer::Renderer(EntityContext &entityContext, Window &window,
                   rhi::RenderDevice *device)
    : mEntityContext(entityContext), mGraphEvaluator(mRegistry),
      mDevice(device), mImguiRenderer(window, mRegistry),
      mSceneRenderer(mEntityContext) {
  loadShaders();
}

Renderer::~Renderer() {
  mEntityContext.destroyComponents<MeshComponent>();
  mEntityContext.destroyComponents<SkinnedMeshComponent>();
  mEntityContext.destroyComponents<SkeletonComponent>();

  mShadowMaterials.clear();
}

void Renderer::render(rhi::RenderGraph &graph) {
  mDevice->execute(graph, mGraphEvaluator);
}

void Renderer::loadShaders() {
  mShaderLibrary.addShader("__engine.geometry.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/geometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.geometry.skinned.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/skinnedGeometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.pbr.default.fragment",
      mRegistry.setShader({Engine::getAssetsPath() + "/shaders/pbr.frag.spv"}));
  mShaderLibrary.addShader("__engine.skybox.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/skybox.vert.spv"}));
  mShaderLibrary.addShader("__engine.skybox.default.fragment",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/skybox.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.default.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/shadowmap.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.skinned.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/skinnedShadowmap.vert.spv"}));

  mShaderLibrary.addShader(
      "__engine.shadowmap.default.fragment",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/shadowmap.frag.spv"}));
  mShaderLibrary.addShader("__engine.imgui.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/imgui.vert.spv"}));
  mShaderLibrary.addShader("__engine.imgui.default.fragment",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/imgui.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.fullscreenQuad.default.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/fullscreenQuad.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.fullscreenQuad.default.fragment",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/fullscreenQuad.frag.spv"}));
}

std::pair<rhi::RenderGraph, DefaultGraphResources>
Renderer::createRenderGraph(const SharedPtr<RenderData> &renderData,
                            bool useSwapchainForImgui) {

  constexpr uint32_t NUM_LIGHTS = 16;
  constexpr uint32_t SHADOWMAP_DIMENSIONS = 2048;
  constexpr glm::vec4 BLUEISH_CLEAR_VALUE{0.19f, 0.21f, 0.26f, 1.0f};
  constexpr uint32_t SWAPCHAIN_SIZE_PERCENTAGE = 100;

  rhi::TextureDescription shadowMapDesc{};
  shadowMapDesc.sizeMethod = rhi::TextureSizeMethod::Fixed;
  shadowMapDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  shadowMapDesc.width = SHADOWMAP_DIMENSIONS;
  shadowMapDesc.height = SHADOWMAP_DIMENSIONS;
  shadowMapDesc.layers = NUM_LIGHTS;
  shadowMapDesc.format = VK_FORMAT_D16_UNORM;
  auto shadowmap = mRegistry.setTexture(shadowMapDesc);

  rhi::TextureDescription mainColorDesc{};
  mainColorDesc.sizeMethod = rhi::TextureSizeMethod::SwapchainRatio;
  mainColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  mainColorDesc.width = SWAPCHAIN_SIZE_PERCENTAGE;
  mainColorDesc.height = SWAPCHAIN_SIZE_PERCENTAGE;
  mainColorDesc.layers = 1;
  mainColorDesc.format = VK_FORMAT_B8G8R8A8_SRGB;
  auto mainColor = mRegistry.setTexture(mainColorDesc);

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = rhi::TextureSizeMethod::SwapchainRatio;
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = SWAPCHAIN_SIZE_PERCENTAGE;
  depthBufferDesc.height = SWAPCHAIN_SIZE_PERCENTAGE;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = VK_FORMAT_D32_SFLOAT;
  auto depthBuffer = mRegistry.setTexture(depthBufferDesc);

  rhi::RenderGraph graph;
  {
    auto &pass = graph.addPass("shadowmap");
    pass.write(shadowmap, rhi::DepthStencilClear{1.0f, 0});

    auto pipeline = mRegistry.setPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.shadowmap.default.vertex"),
        mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                rhi::FrontFace::Clockwise}});

    auto skinnedPipeline = mRegistry.setPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.shadowmap.skinned.vertex"),
        mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
        rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                rhi::FrontFace::Clockwise}});

    pass.addPipeline(pipeline);
    pass.addPipeline(skinnedPipeline);

    pass.setExecutor([pipeline, skinnedPipeline, shadowmap,
                      this](rhi::RenderCommandList &commandList) {
      commandList.bindPipeline(pipeline);

      for (auto &shadowMaterial : mShadowMaterials) {
        commandList.bindDescriptor(pipeline, 0,
                                   shadowMaterial->getDescriptor());

        mSceneRenderer.render(commandList, pipeline, false);
      }

      commandList.bindPipeline(skinnedPipeline);

      for (auto &shadowMaterial : mShadowMaterials) {
        commandList.bindDescriptor(skinnedPipeline, 0,
                                   shadowMaterial->getDescriptor());

        mSceneRenderer.renderSkinned(commandList, skinnedPipeline, 1, false);
      }
    });
  }

  {
    auto &pass = graph.addPass("mainColor");
    pass.read(shadowmap);
    pass.write(mainColor, BLUEISH_CLEAR_VALUE);
    pass.write(depthBuffer, rhi::DepthStencilClear{1.0, 0});

    auto pipeline = mRegistry.setPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.geometry.default.vertex"),
        mShaderLibrary.getShader("__engine.pbr.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::Clockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});

    auto skinnedPipeline = mRegistry.setPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.geometry.skinned.vertex"),
        mShaderLibrary.getShader("__engine.pbr.default.fragment"),
        rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::Clockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});

    pass.addPipeline(pipeline);
    pass.addPipeline(skinnedPipeline);

    pass.setExecutor([this, pipeline, skinnedPipeline, shadowmap,
                      renderData](rhi::RenderCommandList &commandList) {
      commandList.bindPipeline(pipeline);

      auto cameraBuffer =
          renderData->getScene()->getActiveCamera()->getBuffer();

      rhi::Descriptor sceneDescriptor, sceneDescriptorFragment;

      const auto &iblMaps = renderData->getEnvironmentTextures();

      sceneDescriptor.bind(0, cameraBuffer, rhi::DescriptorType::UniformBuffer);
      sceneDescriptorFragment
          .bind(0, cameraBuffer, rhi::DescriptorType::UniformBuffer)
          .bind(1, renderData->getSceneBuffer(),
                rhi::DescriptorType::UniformBuffer)
          .bind(2, {shadowmap}, rhi::DescriptorType::CombinedImageSampler);

      sceneDescriptorFragment
          .bind(3, {iblMaps.at(0), iblMaps.at(1)},
                rhi::DescriptorType::CombinedImageSampler)
          .bind(4, {iblMaps.at(2)}, rhi::DescriptorType::CombinedImageSampler);

      commandList.bindPipeline(pipeline);
      commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(pipeline, 1, sceneDescriptorFragment);

      mSceneRenderer.render(commandList, pipeline, true);

      commandList.bindPipeline(skinnedPipeline);
      commandList.bindDescriptor(skinnedPipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(skinnedPipeline, 1, sceneDescriptorFragment);

      mSceneRenderer.renderSkinned(commandList, skinnedPipeline, 3, true);
    });
  }

  {
    auto &pass = graph.addPass("environmentPass");
    pass.write(mainColor, BLUEISH_CLEAR_VALUE);
    pass.write(depthBuffer, rhi::DepthStencilClear{1.0f, 0});
    auto pipeline = mRegistry.setPipeline(
        {mShaderLibrary.getShader("__engine.skybox.default.vertex"),
         mShaderLibrary.getShader("__engine.skybox.default.fragment"),
         rhi::PipelineVertexInputLayout::create<Vertex>(),
         rhi::PipelineInputAssembly{},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});
    pass.addPipeline(pipeline);
    pass.setExecutor([pipeline, &renderData,
                      this](rhi::RenderCommandList &commandList) {
      commandList.bindPipeline(pipeline);

      rhi::Descriptor descriptor;
      descriptor.bind(0, renderData->getScene()->getActiveCamera()->getBuffer(),
                      rhi::DescriptorType::UniformBuffer);

      commandList.bindDescriptor(pipeline, 0, descriptor);

      mEntityContext.iterateEntities<EnvironmentComponent, MeshComponent>(
          [this, pipeline, &commandList](Entity entity,
                                         const EnvironmentComponent &,
                                         const MeshComponent &mesh) {
            for (size_t i = 0; i < mesh.instance->getVertexBuffers().size();
                 ++i) {
              commandList.bindVertexBuffer(
                  mesh.instance->getVertexBuffers().at(i));
              commandList.bindDescriptor(
                  pipeline, 1,
                  mesh.instance->getMaterials().at(i)->getDescriptor());

              if (rhi::isHandleValid(mesh.instance->getIndexBuffers().at(i))) {
                commandList.bindIndexBuffer(
                    mesh.instance->getIndexBuffers().at(i),
                    VK_INDEX_TYPE_UINT32);
                commandList.drawIndexed(mesh.instance->getIndexCounts().at(i),
                                        0, 0);
              } else {
                commandList.draw(mesh.instance->getVertexCounts().at(i), 0);
              }
            }
          });
    });
  }

  {
    auto &pass = graph.addPass("imgui");

    pass.read(useSwapchainForImgui ? graph.getSwapchain() : mainColor);
    pass.write(graph.getSwapchain(), BLUEISH_CLEAR_VALUE);

    auto pipeline = mRegistry.setPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.imgui.default.vertex"),
        mShaderLibrary.getShader("__engine.imgui.default.fragment"),
        rhi::PipelineVertexInputLayout{
            {rhi::PipelineVertexInputBinding{0, sizeof(ImDrawVert),
                                             rhi::VertexInputRate::Vertex}},
            {rhi::PipelineVertexInputAttribute{0, 0, VK_FORMAT_R32G32_SFLOAT,
                                               IM_OFFSETOF(ImDrawVert, pos)},
             rhi::PipelineVertexInputAttribute{1, 0, VK_FORMAT_R32G32_SFLOAT,
                                               IM_OFFSETOF(ImDrawVert, uv)},
             rhi::PipelineVertexInputAttribute{2, 0, VK_FORMAT_R8G8B8A8_UNORM,
                                               IM_OFFSETOF(ImDrawVert, col)}}

        },
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::CounterClockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
            true, rhi::BlendFactor::SrcAlpha,
            rhi::BlendFactor::OneMinusSrcAlpha, rhi::BlendOp::Add,
            rhi::BlendFactor::One, rhi::BlendFactor::OneMinusSrcAlpha,
            rhi::BlendOp::Add}}}});

    pass.addPipeline(pipeline);

    pass.setExecutor([this, pipeline](rhi::RenderCommandList &commandList) {
      mImguiRenderer.draw(commandList, pipeline);
    });
  }

  return {graph, {mainColor, depthBuffer, shadowmap, BLUEISH_CLEAR_VALUE}};
}

SharedPtr<Material> Renderer::createMaterial(
    const std::vector<rhi::TextureHandle> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const rhi::CullMode &cullMode) {
  return std::make_shared<Material>(textures, properties, mRegistry);
}

SharedPtr<Material>
Renderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                            const rhi::CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties, mRegistry);
}

SharedPtr<RenderData> Renderer::prepareScene(Scene *scene) {
  mShadowMaterials.reserve(
      mEntityContext.getEntityCountForComponent<LightComponent>());

  size_t i = 0;
  mEntityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &light) {
        mShadowMaterials.push_back(SharedPtr<Material>(
            new Material({},
                         {{"lightMatrix", glm::mat4{1.0f}},
                          {"lightIndex", static_cast<int>(i)}},
                         mRegistry)));

        i++;
      });

  return std::make_shared<RenderData>(mEntityContext, scene, mShadowMaterials,
                                      mRegistry);
}

template <class TMeshAsset>
std::vector<SharedPtr<Material>> createMeshMaterials(const TMeshAsset &mesh,
                                                     AssetRegistry &registry,
                                                     Renderer &renderer) {
  auto getTextureFromRegistry = [&registry](TextureAssetHandle handle) {
    if (handle != TextureAssetHandle::Invalid) {
      return registry.getTextures().getAsset(handle).data.deviceHandle;
    }

    return rhi::TextureHandle::Invalid;
  };

  std::vector<SharedPtr<Material>> materials;

  for (auto &geometry : mesh.data.geometries) {
    MaterialPBR::Properties properties;

    if (registry.getMaterials().hasAsset(geometry.material)) {
      auto &material = registry.getMaterials().getAsset(geometry.material).data;

      properties.baseColorFactor = material.baseColorFactor;
      properties.baseColorTexture =
          getTextureFromRegistry(material.baseColorTexture);
      properties.baseColorTextureCoord = material.baseColorTextureCoord;

      properties.metallicFactor = material.metallicFactor;
      properties.metallicRoughnessTexture =
          getTextureFromRegistry(material.metallicRoughnessTexture);
      properties.metallicRoughnessTextureCoord =
          material.metallicRoughnessTextureCoord;

      properties.normalScale = material.normalScale;
      properties.normalTexture = getTextureFromRegistry(material.normalTexture);

      properties.normalTextureCoord = material.normalTextureCoord;

      properties.occlusionStrength = material.occlusionStrength;
      properties.occlusionTexture =
          getTextureFromRegistry(material.occlusionTexture);
      properties.occlusionTextureCoord = material.occlusionTextureCoord;

      properties.emissiveFactor = material.emissiveFactor;
      properties.emissiveTexture =
          getTextureFromRegistry(material.emissiveTexture);
      properties.emissiveTextureCoord = material.emissiveTextureCoord;
    }
    auto materialInstance =
        renderer.createMaterialPBR(properties, rhi::CullMode::None);

    materials.push_back(materialInstance);
  }

  return materials;
}

SharedPtr<MeshInstance> Renderer::createMeshInstance(MeshAssetHandle handle,
                                                     AssetRegistry &registry) {
  const auto &mesh = registry.getMeshes().getAsset(handle);
  const auto &materials = createMeshMaterials(mesh, registry, *this);

  return std::make_shared<MeshInstance>(mesh, materials);
}

SharedPtr<MeshInstance>
Renderer::createMeshInstance(SkinnedMeshAssetHandle handle,
                             AssetRegistry &registry) {

  const auto &mesh = registry.getSkinnedMeshes().getAsset(handle);
  const auto &materials = createMeshMaterials(mesh, registry, *this);

  return std::make_shared<MeshInstance>(mesh, materials);
}

} // namespace liquid
