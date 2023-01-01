#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "SceneRenderer.h"
#include "StandardPushConstants.h"

namespace liquid {

SceneRenderer::SceneRenderer(ShaderLibrary &shaderLibrary,
                             AssetRegistry &assetRegistry,
                             rhi::RenderDevice *device)
    : mShaderLibrary(shaderLibrary), mAssetRegistry(assetRegistry),
      mDevice(device), mFrameData{SceneRendererFrameData(mDevice),
                                  SceneRendererFrameData(mDevice)} {

  auto shadersPath = Engine::getShadersPath();

  mShaderLibrary.addShader(
      "__engine.geometry.default.vertex",
      mDevice->createShader({shadersPath / "geometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.geometry.skinned.vertex",
      mDevice->createShader({shadersPath / "geometry-skinned.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.pbr.default.fragment",
      mDevice->createShader({shadersPath / "pbr.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.skybox.default.vertex",
      mDevice->createShader({shadersPath / "skybox.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.skybox.default.fragment",
      mDevice->createShader({shadersPath / "skybox.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.default.vertex",
      mDevice->createShader({shadersPath / "shadowmap.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.skinned.vertex",
      mDevice->createShader({shadersPath / "shadowmap-skinned.vert.spv"}));

  mShaderLibrary.addShader(
      "__engine.shadowmap.default.fragment",
      mDevice->createShader({shadersPath / "shadowmap.frag.spv"}));

  mShaderLibrary.addShader(
      "__engine.text.default.vertex",
      mDevice->createShader({shadersPath / "text.vert.spv"}));

  mShaderLibrary.addShader(
      "__engine.text.default.fragment",
      mDevice->createShader({shadersPath / "text.frag.spv"}));
}

void SceneRenderer::setClearColor(const glm::vec4 &clearColor) {
  mClearColor = clearColor;
}

SceneRenderPassData SceneRenderer::attach(RenderGraph &graph) {
  static constexpr uint32_t ShadowMapDimensions = 4096;
  static constexpr uint32_t SwapchainSizePercentage = 100;

  rhi::TextureDescription shadowMapDesc{};
  shadowMapDesc.sizeMethod = rhi::TextureSizeMethod::Fixed;
  shadowMapDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  shadowMapDesc.width = ShadowMapDimensions;
  shadowMapDesc.height = ShadowMapDimensions;
  shadowMapDesc.layers = SceneRendererFrameData::MaxShadowMaps;
  shadowMapDesc.format = rhi::Format::Depth16Unorm;
  auto shadowmap = mDevice->createTexture(shadowMapDesc);

  rhi::TextureDescription sceneColorDesc{};
  sceneColorDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  sceneColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  sceneColorDesc.width = SwapchainSizePercentage;
  sceneColorDesc.height = SwapchainSizePercentage;
  sceneColorDesc.layers = 1;
  sceneColorDesc.format = rhi::Format::Bgra8Srgb;
  auto sceneColor = mDevice->createTexture(sceneColorDesc);

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = SwapchainSizePercentage;
  depthBufferDesc.height = SwapchainSizePercentage;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = rhi::Format::Depth32Float;
  auto depthBuffer = mDevice->createTexture(depthBufferDesc);

  {
    auto &pass = graph.addPass("shadowPass");
    pass.write(shadowmap, rhi::DepthStencilClear{1.0f, 0});

    auto vPipeline = pass.addPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.shadowmap.default.vertex"),
        mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                rhi::FrontFace::Clockwise}});

    auto vSkinnedPipeline = pass.addPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.shadowmap.skinned.vertex"),
        mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
        rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                rhi::FrontFace::Clockwise}});

    pass.setExecutor([vPipeline, vSkinnedPipeline, shadowmap,
                      this](rhi::RenderCommandList &commandList,
                            const RenderGraphRegistry &registry,
                            uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);
      rhi::Descriptor descriptor;
      descriptor.bind(0, frameData.getShadowMapsBuffer(),
                      rhi::DescriptorType::StorageBuffer);

      auto pipeline = registry.get(vPipeline);
      auto skinnedPipeline = registry.get(vSkinnedPipeline);

      {
        LIQUID_PROFILE_EVENT("shadowPass::meshes");
        commandList.bindPipeline(pipeline);

        commandList.bindDescriptor(pipeline, 0, descriptor);

        for (int32_t index = 0;
             index < static_cast<int32_t>(frameData.getNumShadowMaps());
             ++index) {
          glm::ivec4 pcIndex{index};

          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(glm::ivec4), &pcIndex);
          render(commandList, pipeline, false, frameIndex);
        }
      }

      {
        LIQUID_PROFILE_EVENT("shadowPass::skinnedMeshes");
        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(skinnedPipeline, 0, descriptor);

        for (int32_t index = 0;
             index < static_cast<int32_t>(frameData.getNumShadowMaps());
             ++index) {
          glm::ivec4 pcIndex{index};

          commandList.pushConstants(skinnedPipeline, rhi::ShaderStage::Vertex,
                                    0, sizeof(glm::ivec4), &pcIndex);
          renderSkinned(commandList, skinnedPipeline, false, frameIndex);
        }
      }
    });
  } // shadow pass

  {
    auto &pass = graph.addPass("meshPass");
    pass.read(shadowmap);
    pass.write(sceneColor, mClearColor);
    pass.write(depthBuffer, rhi::DepthStencilClear{1.0, 0});

    auto vPipeline = pass.addPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.geometry.default.vertex"),
        mShaderLibrary.getShader("__engine.pbr.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::Clockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});

    auto vSkinnedPipeline = pass.addPipeline(rhi::PipelineDescription{
        mShaderLibrary.getShader("__engine.geometry.skinned.vertex"),
        mShaderLibrary.getShader("__engine.pbr.default.fragment"),
        rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::Clockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});

    pass.setExecutor([this, vPipeline, vSkinnedPipeline,
                      shadowmap](rhi::RenderCommandList &commandList,
                                 const RenderGraphRegistry &registry,
                                 uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);
      auto pipeline = registry.get(vPipeline);
      auto skinnedPipeline = registry.get(vSkinnedPipeline);

      commandList.bindPipeline(pipeline);

      rhi::Descriptor sceneDescriptor, sceneDescriptorFragment;

      static constexpr uint32_t IrradianceBinding = 5;
      static constexpr uint32_t BrdfBinding = 6;

      sceneDescriptor.bind(0, frameData.getActiveCameraBuffer(),
                           rhi::DescriptorType::UniformBuffer);
      sceneDescriptorFragment
          .bind(0, frameData.getActiveCameraBuffer(),
                rhi::DescriptorType::UniformBuffer)
          .bind(1, frameData.getSceneBuffer(),
                rhi::DescriptorType::UniformBuffer)
          .bind(2, frameData.getLightsBuffer(),
                rhi::DescriptorType::StorageBuffer)
          .bind(3, frameData.getShadowMapsBuffer(),
                rhi::DescriptorType::StorageBuffer)
          .bind(4, {shadowmap}, rhi::DescriptorType::CombinedImageSampler)
          .bind(IrradianceBinding,
                {frameData.getIrradianceMap(), frameData.getSpecularMap()},
                rhi::DescriptorType::CombinedImageSampler)
          .bind(BrdfBinding, {frameData.getBrdfLUT()},
                rhi::DescriptorType::CombinedImageSampler);

      {
        LIQUID_PROFILE_EVENT("meshPass::meshes");

        commandList.bindPipeline(pipeline);
        commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
        commandList.bindDescriptor(pipeline, 2, sceneDescriptorFragment);

        render(commandList, pipeline, true, frameIndex);
      }

      {
        LIQUID_PROFILE_EVENT("meshPass::skinnedMeshes");

        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(skinnedPipeline, 0, sceneDescriptor);
        commandList.bindDescriptor(skinnedPipeline, 2, sceneDescriptorFragment);

        renderSkinned(commandList, skinnedPipeline, true, frameIndex);
      }
    });
  } // mesh pass

  {
    auto &pass = graph.addPass("environmentPass");
    pass.write(sceneColor, mClearColor);
    pass.write(depthBuffer, rhi::DepthStencilClear{1.0f, 0});
    auto vPipeline = pass.addPipeline(
        {mShaderLibrary.getShader("__engine.skybox.default.vertex"),
         mShaderLibrary.getShader("__engine.skybox.default.fragment"),
         rhi::PipelineVertexInputLayout::create<Vertex>(),
         rhi::PipelineInputAssembly{},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});
    pass.setExecutor([vPipeline, this](rhi::RenderCommandList &commandList,
                                       const RenderGraphRegistry &registry,
                                       uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);
      if (!rhi::isHandleValid(frameData.getIrradianceMap()))
        return;

      auto pipeline = registry.get(vPipeline);

      commandList.bindPipeline(pipeline);

      rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, frameData.getActiveCameraBuffer(),
                           rhi::DescriptorType::UniformBuffer);

      rhi::Descriptor skyboxDescriptor;
      skyboxDescriptor.bind(0, {frameData.getIrradianceMap()},
                            rhi::DescriptorType::CombinedImageSampler);

      commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(pipeline, 1, skyboxDescriptor);

      const auto &cube = mAssetRegistry.getMeshes()
                             .getAsset(mAssetRegistry.getDefaultObjects().cube)
                             .data;

      commandList.bindVertexBuffer(cube.vertexBuffers.at(0).getHandle());
      commandList.bindIndexBuffer(cube.indexBuffers.at(0).getHandle(),
                                  rhi::IndexType::Uint32);
      commandList.drawIndexed(
          static_cast<uint32_t>(cube.geometries.at(0).indices.size()), 0, 0);
    });
  } // environment pass

  LOG_DEBUG("Scene renderer attached to graph");

  return SceneRenderPassData{sceneColor, depthBuffer};
}

void SceneRenderer::attachText(RenderGraph &graph,
                               const SceneRenderPassData &passData) {
  auto &pass = graph.addPass("textPass");
  pass.write(passData.sceneColor, mClearColor);
  pass.write(passData.depthBuffer, rhi::DepthStencilClear{1.0f, 0});

  auto vTextPipeline = pass.addPipeline(rhi::PipelineDescription{
      mShaderLibrary.getShader("__engine.text.default.vertex"),
      mShaderLibrary.getShader("__engine.text.default.fragment"),
      rhi::PipelineVertexInputLayout{},
      rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
      rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                              rhi::FrontFace::Clockwise},
      rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
          true, rhi::BlendFactor::SrcAlpha, rhi::BlendFactor::OneMinusSrcAlpha,
          rhi::BlendOp::Add, rhi::BlendFactor::One,
          rhi::BlendFactor::OneMinusSrcAlpha, rhi::BlendOp::Add}}}});

  pass.setExecutor([vTextPipeline, this](rhi::RenderCommandList &commandList,
                                         const RenderGraphRegistry &registry,
                                         uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    auto textPipeline = registry.get(vTextPipeline);

    commandList.bindPipeline(textPipeline);
    rhi::Descriptor sceneDescriptor;
    sceneDescriptor.bind(0, frameData.getActiveCameraBuffer(),
                         rhi::DescriptorType::UniformBuffer);
    commandList.bindDescriptor(textPipeline, 0, sceneDescriptor);
    renderText(commandList, textPipeline, frameIndex);
  });
}

void SceneRenderer::updateFrameData(EntityDatabase &entityDatabase,
                                    Entity camera, uint32_t frameIndex) {
  LIQUID_ASSERT(entityDatabase.has<Camera>(camera),
                "Entity does not have a camera");

  auto &frameData = mFrameData.at(frameIndex);

  LIQUID_PROFILE_EVENT("SceneRenderer::updateFrameData");
  frameData.clear();

  frameData.setCameraData(entityDatabase.get<Camera>(camera),
                          entityDatabase.get<PerspectiveLens>(camera));

  // Meshes
  for (auto [entity, world, mesh] :
       entityDatabase.view<WorldTransform, Mesh>()) {
    frameData.addMesh(mesh.handle, entity, world.worldTransform);
  }

  // Skinned Meshes
  for (auto [entity, skeleton, world, mesh] :
       entityDatabase.view<Skeleton, WorldTransform, SkinnedMesh>()) {
    frameData.addSkinnedMesh(mesh.handle, entity, world.worldTransform,
                             skeleton.jointFinalTransforms);
  }

  // Texts
  for (auto [entity, text, world] :
       entityDatabase.view<Text, WorldTransform>()) {
    const auto &font = mAssetRegistry.getFonts().getAsset(text.font).data;

    std::vector<SceneRendererFrameData::GlyphData> glyphs(text.text.length());
    float advanceX = 0;
    float advanceY = 0;
    for (size_t i = 0; i < text.text.length(); ++i) {
      char c = text.text.at(i);

      if (c == '\n') {
        advanceX = 0.0f;
        advanceY += text.lineHeight * font.fontScale;
        continue;
      }

      const auto &fontGlyph = font.glyphs.at(c);
      glyphs.at(i).bounds = fontGlyph.bounds;
      glyphs.at(i).planeBounds = fontGlyph.planeBounds;

      glyphs.at(i).planeBounds.x += advanceX;
      glyphs.at(i).planeBounds.z += advanceX;
      glyphs.at(i).planeBounds.y -= advanceY;
      glyphs.at(i).planeBounds.w -= advanceY;

      advanceX += fontGlyph.advanceX;
    }

    frameData.addText(text.font, glyphs, world.worldTransform);
  }

  // Lights
  for (auto [entity, light] : entityDatabase.view<DirectionalLight>()) {
    if (entityDatabase.has<CascadedShadowMap>(entity)) {
      frameData.addLight(light, entityDatabase.get<CascadedShadowMap>(entity));
    } else {
      frameData.addLight(light);
    }
  };

  // Environments
  for (auto [entity, environment] : entityDatabase.view<Environment>()) {
    frameData.setEnvironmentTextures(environment.irradianceMap,
                                     environment.specularMap,
                                     environment.brdfLUT);
  }

  frameData.updateBuffers();
}

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline, bool bindMaterialData,
                           uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);
  rhi::Descriptor descriptor;
  descriptor.bind(0, frameData.getMeshTransformsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  commandList.bindDescriptor(pipeline, 1, descriptor);

  uint32_t instanceStart = 0;

  for (auto &[handle, meshData] : frameData.getMeshGroups()) {
    const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;
    for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
      commandList.bindVertexBuffer(mesh.vertexBuffers.at(g).getHandle());
      bool indexed = rhi::isHandleValid(mesh.indexBuffers.at(g).getHandle());
      if (indexed) {
        commandList.bindIndexBuffer(mesh.indexBuffers.at(g).getHandle(),
                                    rhi::IndexType::Uint32);
      }

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 3,
                                   mesh.materials.at(g)->getDescriptor());
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      uint32_t vertexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

      if (indexed) {
        commandList.drawIndexed(
            indexCount, 0, 0, static_cast<uint32_t>(meshData.transforms.size()),
            instanceStart);
      } else {
        commandList.draw(vertexCount, 0,
                         static_cast<uint32_t>(meshData.transforms.size()),
                         instanceStart);
      }

      instanceStart += static_cast<uint32_t>(meshData.transforms.size());
    }
  }
}

void SceneRenderer::renderSkinned(rhi::RenderCommandList &commandList,
                                  rhi::PipelineHandle pipeline,
                                  bool bindMaterialData, uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  rhi::Descriptor descriptor;
  descriptor.bind(0, frameData.getSkinnedMeshTransformsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  descriptor.bind(1, frameData.getSkeletonsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  commandList.bindDescriptor(pipeline, 1, descriptor);

  uint32_t instanceStart = 0;

  for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
    const auto &mesh = mAssetRegistry.getSkinnedMeshes().getAsset(handle).data;
    for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
      commandList.bindVertexBuffer(mesh.vertexBuffers.at(g).getHandle());
      bool indexed = rhi::isHandleValid(mesh.indexBuffers.at(g).getHandle());
      if (indexed) {
        commandList.bindIndexBuffer(mesh.indexBuffers.at(g).getHandle(),
                                    rhi::IndexType::Uint32);
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      uint32_t vertexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 3,
                                   mesh.materials.at(g)->getDescriptor());
      }

      uint32_t numInstances = static_cast<uint32_t>(meshData.transforms.size());

      if (indexed) {
        commandList.drawIndexed(indexCount, 0, 0, numInstances, instanceStart);
      } else {
        commandList.draw(vertexCount, 0, numInstances, instanceStart);
      }

      instanceStart += numInstances;
    }
  }
}

void SceneRenderer::renderText(rhi::RenderCommandList &commandList,
                               rhi::PipelineHandle pipeline,
                               uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);
  static constexpr uint32_t QuadNumVertices = 6;
  for (const auto &[font, texts] : frameData.getTextGroups()) {
    auto textureHandle =
        mAssetRegistry.getFonts().getAsset(font).data.deviceHandle;

    rhi::Descriptor objectsDescriptor;
    objectsDescriptor.bind(0, frameData.getTextTransformsBuffer(),
                           rhi::DescriptorType::StorageBuffer);

    rhi::Descriptor fontDescriptor;
    fontDescriptor.bind(0, {textureHandle},
                        rhi::DescriptorType::CombinedImageSampler);

    rhi::Descriptor glyphsDescriptor;
    glyphsDescriptor.bind(0, frameData.getTextGlyphsBuffer(),
                          rhi::DescriptorType::StorageBuffer);

    commandList.bindDescriptor(pipeline, 1, objectsDescriptor);
    commandList.bindDescriptor(pipeline, 2, glyphsDescriptor);
    commandList.bindDescriptor(pipeline, 3, fontDescriptor);

    for (auto &text : texts) {
      glm::uvec4 glyphStart{text.glyphStart};

      commandList.pushConstants(
          pipeline, rhi::ShaderStage::Vertex, 0, sizeof(glm::uvec4),
          static_cast<void *>(glm::value_ptr(glyphStart)));

      commandList.draw(QuadNumVertices * static_cast<uint32_t>(text.length), 0,
                       1, text.index);
    }
  }
}

} // namespace liquid
