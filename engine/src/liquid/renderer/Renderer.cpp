#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "liquid/renderer/SceneRenderer.h"

#include "liquid/rhi/RenderGraph.h"

namespace liquid {

Renderer::Renderer(AssetRegistry &assetRegistry, Window &window,
                   rhi::RenderDevice *device)
    : mGraphEvaluator(mRegistry), mDevice(device),
      mImguiRenderer(window, mRegistry), mAssetRegistry(assetRegistry) {
  loadShaders();
}

void Renderer::render(rhi::RenderGraph &graph, Entity camera,
                      EntityContext &entityContext) {
  LIQUID_ASSERT(entityContext.hasComponent<CameraComponent>(camera),
                "Entity does not have a camera");
  mRenderStorage.setCameraData(
      entityContext.getComponent<CameraComponent>(camera));

  updateStorageBuffers(entityContext);
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

void Renderer::updateStorageBuffers(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("Renderer::updateStorageBuffers");
  mRenderStorage.clear();

  // Meshes
  entityContext.iterateEntities<WorldTransformComponent, MeshComponent>(
      [this](auto entity, const auto &world, const auto &mesh) {
        mRenderStorage.addMesh(mesh.handle, world.worldTransform);
      });

  // Skinned Meshes
  entityContext.iterateEntities<SkeletonComponent, WorldTransformComponent,
                                SkinnedMeshComponent>(
      [this](auto entity, const auto &skeleton, const auto &world,
             const auto &mesh) {
        mRenderStorage.addSkinnedMesh(
            mesh.handle, world.worldTransform,
            skeleton.skeleton.getJointFinalTransforms());
      });

  // Lights
  entityContext.iterateEntities<DirectionalLightComponent>(
      [this](auto entity, const auto &light) {
        mRenderStorage.addLight(light);
      });

  // Environments
  entityContext.iterateEntities<EnvironmentComponent>(
      [this](auto entity, const auto &environment) {
        mRenderStorage.setEnvironmentTextures(environment.irradianceMap,
                                              environment.specularMap,
                                              environment.brdfLUT);
      });

  mRenderStorage.updateBuffers(mRegistry);
}

std::pair<rhi::RenderGraph, DefaultGraphResources>
Renderer::createRenderGraph(bool useSwapchainForImgui) {
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

      rhi::Descriptor descriptor;
      descriptor.bind(0, mRenderStorage.getLightsBuffer(),
                      rhi::DescriptorType::StorageBuffer);

      commandList.bindDescriptor(pipeline, 0, descriptor);

      for (int32_t index = 0; index < mRenderStorage.getNumLights(); ++index) {
        glm::ivec4 pcIndex{index};

        commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                  sizeof(glm::ivec4), &pcIndex);
        mSceneRenderer.render(commandList, pipeline, mRenderStorage,
                              mAssetRegistry, false);
      }

      commandList.bindPipeline(skinnedPipeline);
      commandList.bindDescriptor(skinnedPipeline, 0, descriptor);

      for (int32_t index = 0; index < mRenderStorage.getNumLights(); ++index) {
        glm::ivec4 pcIndex{index};

        commandList.pushConstants(skinnedPipeline, VK_SHADER_STAGE_VERTEX_BIT,
                                  0, sizeof(glm::ivec4), &pcIndex);
        mSceneRenderer.renderSkinned(commandList, skinnedPipeline,
                                     mRenderStorage, mAssetRegistry, false);
        index++;
      }
    });
  } // shadowmap

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

    pass.setExecutor([this, pipeline, skinnedPipeline,
                      shadowmap](rhi::RenderCommandList &commandList) {
      commandList.bindPipeline(pipeline);

      rhi::Descriptor sceneDescriptor, sceneDescriptorFragment;

      static constexpr uint32_t BRDF_BINDING = 5;

      sceneDescriptor.bind(0, mRenderStorage.getActiveCameraBuffer(),
                           rhi::DescriptorType::UniformBuffer);
      sceneDescriptorFragment
          .bind(0, mRenderStorage.getActiveCameraBuffer(),
                rhi::DescriptorType::UniformBuffer)
          .bind(1, mRenderStorage.getSceneBuffer(),
                rhi::DescriptorType::UniformBuffer)
          .bind(2, mRenderStorage.getLightsBuffer(),
                rhi::DescriptorType::StorageBuffer)
          .bind(3, {shadowmap}, rhi::DescriptorType::CombinedImageSampler)
          .bind(4,
                {mRenderStorage.getIrradianceMap(),
                 mRenderStorage.getSpecularMap()},
                rhi::DescriptorType::CombinedImageSampler)
          .bind(BRDF_BINDING, {mRenderStorage.getBrdfLUT()},
                rhi::DescriptorType::CombinedImageSampler);

      commandList.bindPipeline(pipeline);
      commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(pipeline, 2, sceneDescriptorFragment);

      mSceneRenderer.render(commandList, pipeline, mRenderStorage,
                            mAssetRegistry, true);

      commandList.bindPipeline(skinnedPipeline);
      commandList.bindDescriptor(skinnedPipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(skinnedPipeline, 2, sceneDescriptorFragment);

      mSceneRenderer.renderSkinned(commandList, skinnedPipeline, mRenderStorage,
                                   mAssetRegistry, true);
    });
  } // scene

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
    pass.setExecutor([pipeline, this](rhi::RenderCommandList &commandList) {
      if (!rhi::isHandleValid(mRenderStorage.getIrradianceMap()))
        return;

      commandList.bindPipeline(pipeline);

      rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, mRenderStorage.getActiveCameraBuffer(),
                           rhi::DescriptorType::UniformBuffer);

      rhi::Descriptor skyboxDescriptor;
      skyboxDescriptor.bind(0, {mRenderStorage.getIrradianceMap()},
                            rhi::DescriptorType::CombinedImageSampler);

      commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(pipeline, 1, skyboxDescriptor);

      const auto &cube = mAssetRegistry.getMeshes()
                             .getAsset(mAssetRegistry.getDefaultObjects().cube)
                             .data;

      commandList.bindVertexBuffer(cube.vertexBuffers.at(0));
      commandList.bindIndexBuffer(cube.indexBuffers.at(0),
                                  VK_INDEX_TYPE_UINT32);
      commandList.drawIndexed(
          static_cast<uint32_t>(cube.geometries.at(0).indices.size()), 0, 0);
    });
  } // environment

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
  } // imgui

  return {graph, {mainColor, depthBuffer, shadowmap, BLUEISH_CLEAR_VALUE}};
}

} // namespace liquid
