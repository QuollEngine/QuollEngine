#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "SceneRenderer.h"

namespace liquid {

SceneRenderer::SceneRenderer(ShaderLibrary &shaderLibrary,
                             rhi::ResourceRegistry &resourceRegistry,
                             AssetRegistry &assetRegistry)
    : mShaderLibrary(shaderLibrary), mRegistry(resourceRegistry),
      mAssetRegistry(assetRegistry) {

  auto assetsPath = Engine::getAssetsPath();

  mShaderLibrary.addShader(
      "__engine.geometry.default.vertex",
      mRegistry.setShader({assetsPath + "/shaders/geometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.geometry.skinned.vertex",
      mRegistry.setShader({assetsPath + "/shaders/skinnedGeometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.pbr.default.fragment",
      mRegistry.setShader({assetsPath + "/shaders/pbr.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.skybox.default.vertex",
      mRegistry.setShader({assetsPath + "/shaders/skybox.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.skybox.default.fragment",
      mRegistry.setShader({assetsPath + "/shaders/skybox.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.default.vertex",
      mRegistry.setShader({assetsPath + "/shaders/shadowmap.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.skinned.vertex",
      mRegistry.setShader({assetsPath + "/shaders/skinnedShadowmap.vert.spv"}));

  mShaderLibrary.addShader(
      "__engine.shadowmap.default.fragment",
      mRegistry.setShader({assetsPath + "/shaders/shadowmap.frag.spv"}));
}

SceneRenderPassData SceneRenderer::attach(rhi::RenderGraph &graph) {
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

  rhi::TextureDescription sceneColorDesc{};
  sceneColorDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  sceneColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  sceneColorDesc.width = SWAPCHAIN_SIZE_PERCENTAGE;
  sceneColorDesc.height = SWAPCHAIN_SIZE_PERCENTAGE;
  sceneColorDesc.layers = 1;
  sceneColorDesc.format = VK_FORMAT_B8G8R8A8_SRGB;
  auto sceneColor = mRegistry.setTexture(sceneColorDesc);

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = SWAPCHAIN_SIZE_PERCENTAGE;
  depthBufferDesc.height = SWAPCHAIN_SIZE_PERCENTAGE;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = VK_FORMAT_D32_SFLOAT;
  auto depthBuffer = mRegistry.setTexture(depthBufferDesc);

  {
    auto &pass = graph.addPass("shadowPass");
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
      rhi::Descriptor descriptor;
      descriptor.bind(0, mRenderStorage.getLightsBuffer(),
                      rhi::DescriptorType::StorageBuffer);

      {
        LIQUID_PROFILE_EVENT("shadowPass::meshes");
        commandList.bindPipeline(pipeline);

        commandList.bindDescriptor(pipeline, 0, descriptor);

        for (int32_t index = 0; index < mRenderStorage.getNumLights();
             ++index) {
          glm::ivec4 pcIndex{index};

          commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                    sizeof(glm::ivec4), &pcIndex);
          render(commandList, pipeline, false);
        }
      }

      {
        LIQUID_PROFILE_EVENT("shadowPass::skinnedMeshes");
        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(skinnedPipeline, 0, descriptor);

        for (int32_t index = 0; index < mRenderStorage.getNumLights();
             ++index) {
          glm::ivec4 pcIndex{index};

          commandList.pushConstants(skinnedPipeline, VK_SHADER_STAGE_VERTEX_BIT,
                                    0, sizeof(glm::ivec4), &pcIndex);
          renderSkinned(commandList, skinnedPipeline, false);
          index++;
        }
      }
    });
  } // shadow pass

  {
    auto &pass = graph.addPass("meshPass");
    pass.read(shadowmap);
    pass.write(sceneColor, BLUEISH_CLEAR_VALUE);
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

      {
        LIQUID_PROFILE_EVENT("meshPass::meshes");

        commandList.bindPipeline(pipeline);
        commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
        commandList.bindDescriptor(pipeline, 2, sceneDescriptorFragment);

        render(commandList, pipeline, true);
      }

      {
        LIQUID_PROFILE_EVENT("meshPass::skinnedMeshes");

        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(skinnedPipeline, 0, sceneDescriptor);
        commandList.bindDescriptor(skinnedPipeline, 2, sceneDescriptorFragment);

        renderSkinned(commandList, skinnedPipeline, true);
      }
    });
  } // mesh pass

  {
    auto &pass = graph.addPass("environmentPass");
    pass.write(sceneColor, BLUEISH_CLEAR_VALUE);
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
  } // environment pass

  return SceneRenderPassData{sceneColor, depthBuffer};
}

void SceneRenderer::updateFrameData(EntityContext &entityContext,
                                    Entity camera) {
  LIQUID_ASSERT(entityContext.hasComponent<CameraComponent>(camera),
                "Entity does not have a camera");

  LIQUID_PROFILE_EVENT("SceneRenderer::updateFrameData");
  mRenderStorage.clear();

  mRenderStorage.setCameraData(
      entityContext.getComponent<CameraComponent>(camera));

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
        mRenderStorage.addSkinnedMesh(mesh.handle, world.worldTransform,
                                      skeleton.jointFinalTransforms);
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

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline,
                           bool bindMaterialData) {
  rhi::Descriptor descriptor;
  descriptor.bind(0, mRenderStorage.getMeshTransformsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  commandList.bindDescriptor(pipeline, 1, descriptor);

  for (auto &[handle, meshData] : mRenderStorage.getMeshGroups()) {
    const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;
    for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
      commandList.bindVertexBuffer(mesh.vertexBuffers.at(g));
      bool indexed = rhi::isHandleValid(mesh.indexBuffers.at(g));
      if (indexed) {
        commandList.bindIndexBuffer(mesh.indexBuffers.at(g),
                                    VK_INDEX_TYPE_UINT32);
      }

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 3,
                                   mesh.materials.at(g)->getDescriptor());
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
}

void SceneRenderer::renderSkinned(rhi::RenderCommandList &commandList,
                                  rhi::PipelineHandle pipeline,
                                  bool bindMaterialData) {
  rhi::Descriptor descriptor;
  descriptor.bind(0, mRenderStorage.getSkinnedMeshTransformsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  descriptor.bind(1, mRenderStorage.getSkeletonsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  commandList.bindDescriptor(pipeline, 1, descriptor);

  uint32_t index = 0;

  for (auto &[handle, meshData] : mRenderStorage.getSkinnedMeshGroups()) {
    const auto &mesh = mAssetRegistry.getSkinnedMeshes().getAsset(handle).data;
    for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
      commandList.bindVertexBuffer(mesh.vertexBuffers.at(g));
      bool indexed = rhi::isHandleValid(mesh.indexBuffers.at(g));
      if (indexed) {
        commandList.bindIndexBuffer(mesh.indexBuffers.at(g),
                                    VK_INDEX_TYPE_UINT32);
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      uint32_t vertexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 3,
                                   mesh.materials.at(g)->getDescriptor());
      }

      for (auto index : meshData.indices) {
        if (indexed) {
          commandList.drawIndexed(indexCount, 0, 0, 1, index);
        } else {
          commandList.draw(vertexCount, 0, 1, index);
        }
      }
    }
  }
}

} // namespace liquid
