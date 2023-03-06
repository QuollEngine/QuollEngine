#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "SceneRenderer.h"
#include "StandardPushConstants.h"

namespace liquid {

SceneRenderer::SceneRenderer(ShaderLibrary &shaderLibrary,
                             AssetRegistry &assetRegistry,
                             RenderStorage &renderStorage,
                             rhi::RenderDevice *device)
    : mShaderLibrary(shaderLibrary), mAssetRegistry(assetRegistry),
      mDevice(device), mRenderStorage(renderStorage),
      mFrameData{SceneRendererFrameData(renderStorage, mDevice),
                 SceneRendererFrameData(renderStorage, mDevice)} {

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

  mShaderLibrary.addShader(
      "__engine.pbr.brdfLut.compute",
      mDevice->createShader({shadersPath / "generate-brdf-lut.comp.spv"}));

  generateBrdfLut();
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
  auto shadowmap = mRenderStorage.createTexture(shadowMapDesc);

  for (auto &frameData : mFrameData) {
    frameData.setShadowMapTexture(shadowmap);
  }

  rhi::TextureDescription sceneColorDesc{};
  sceneColorDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  sceneColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  sceneColorDesc.width = SwapchainSizePercentage;
  sceneColorDesc.height = SwapchainSizePercentage;
  sceneColorDesc.layers = 1;
  sceneColorDesc.format = rhi::Format::Bgra8Srgb;
  auto sceneColor = mRenderStorage.createTexture(sceneColorDesc);

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = rhi::TextureSizeMethod::FramebufferRatio;
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = SwapchainSizePercentage;
  depthBufferDesc.height = SwapchainSizePercentage;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = rhi::Format::Depth32Float;
  auto depthBuffer = mRenderStorage.createTexture(depthBufferDesc);

  {
    auto &pass = graph.addGraphicsPass("shadowPass");
    pass.write(shadowmap, rhi::DepthStencilClear{1.0f, 0});

    auto vPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
        mShaderLibrary.getShader("__engine.shadowmap.default.vertex"),
        mShaderLibrary.getShader("__engine.shadowmap.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                rhi::FrontFace::Clockwise}});

    auto vSkinnedPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
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

      auto pipeline = registry.get(vPipeline);
      auto skinnedPipeline = registry.get(vSkinnedPipeline);

      {
        LIQUID_PROFILE_EVENT("shadowPass::meshes");
        commandList.bindPipeline(pipeline);
        commandList.bindDescriptor(pipeline, 0,
                                   mRenderStorage.getGlobalBuffersDescriptor());

        for (int32_t index = 0;
             index < static_cast<int32_t>(frameData.getNumShadowMaps());
             ++index) {
          frameData.getDrawParams().index9 = index;

          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(DrawParameters),
                                    &frameData.getDrawParams());

          render(commandList, pipeline, false, frameIndex);
        }
      }

      {
        LIQUID_PROFILE_EVENT("shadowPass::skinnedMeshes");
        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(skinnedPipeline, 0,
                                   mRenderStorage.getGlobalBuffersDescriptor());

        for (int32_t index = 0;
             index < static_cast<int32_t>(frameData.getNumShadowMaps());
             ++index) {
          frameData.getDrawParams().index9 = index;

          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(DrawParameters),
                                    &frameData.getDrawParams());
          renderSkinned(commandList, skinnedPipeline, false, frameIndex);
        }
      }
    });
  } // shadow pass

  {
    auto &pass = graph.addGraphicsPass("meshPass");
    pass.read(shadowmap);
    pass.write(sceneColor, mClearColor);
    pass.write(depthBuffer, rhi::DepthStencilClear{1.0, 0});

    auto vPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
        mShaderLibrary.getShader("__engine.geometry.default.vertex"),
        mShaderLibrary.getShader("__engine.pbr.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::Clockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}}});

    auto vSkinnedPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
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

      {
        LIQUID_PROFILE_EVENT("meshPass::meshes");

        commandList.bindPipeline(pipeline);
        commandList.bindDescriptor(pipeline, 0,
                                   mRenderStorage.getGlobalBuffersDescriptor());
        commandList.bindDescriptor(
            pipeline, 1, mRenderStorage.getGlobalTexturesDescriptor());

        commandList.pushConstants(
            pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
            sizeof(DrawParameters), &frameData.getDrawParams());

        render(commandList, pipeline, true, frameIndex);
      }

      {
        LIQUID_PROFILE_EVENT("meshPass::skinnedMeshes");

        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(skinnedPipeline, 0,
                                   mRenderStorage.getGlobalBuffersDescriptor());
        commandList.bindDescriptor(
            skinnedPipeline, 1, mRenderStorage.getGlobalTexturesDescriptor());

        commandList.pushConstants(
            skinnedPipeline,
            rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
            sizeof(DrawParameters), &frameData.getDrawParams());

        renderSkinned(commandList, skinnedPipeline, true, frameIndex);
      }
    });
  } // mesh pass

  {
    auto &pass = graph.addGraphicsPass("skyboxPass");
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
      auto pipeline = registry.get(vPipeline);

      commandList.bindPipeline(pipeline);

      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalBuffersDescriptor());
      commandList.bindDescriptor(pipeline, 1,
                                 mRenderStorage.getGlobalTexturesDescriptor());
      commandList.pushConstants(
          pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
          sizeof(DrawParameters), &frameData.getDrawParams());

      const auto &cube = mAssetRegistry.getMeshes()
                             .getAsset(mAssetRegistry.getDefaultObjects().cube)
                             .data;

      commandList.bindVertexBuffer(cube.vertexBuffer.getHandle());
      commandList.bindIndexBuffer(cube.indexBuffer.getHandle(),
                                  rhi::IndexType::Uint32);

      commandList.drawIndexed(
          static_cast<uint32_t>(cube.geometries.at(0).indices.size()), 0, 0);
    });
  } // skybox pass

  LOG_DEBUG("Scene renderer attached to graph");

  return SceneRenderPassData{sceneColor, depthBuffer};
}

void SceneRenderer::attachText(RenderGraph &graph,
                               const SceneRenderPassData &passData) {
  auto &pass = graph.addGraphicsPass("textPass");
  pass.write(passData.sceneColor, mClearColor);
  pass.write(passData.depthBuffer, rhi::DepthStencilClear{1.0f, 0});

  auto vTextPipeline = pass.addPipeline(rhi::GraphicsPipelineDescription{
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
  const auto &textures = mAssetRegistry.getTextures();
  for (auto [entity, environment] : entityDatabase.view<EnvironmentSkybox>()) {
    rhi::TextureHandle irradianceMap{0};
    rhi::TextureHandle specularMap{0};

    if (environment.type == EnvironmentSkyboxType::Color) {
      frameData.setSkyboxColor(environment.color);
    } else if (environment.type == EnvironmentSkyboxType::Texture &&
               mAssetRegistry.getEnvironments().hasAsset(environment.texture)) {
      const auto &asset =
          mAssetRegistry.getEnvironments().getAsset(environment.texture).data;

      frameData.setSkyboxTexture(
          textures.getAsset(asset.specularMap).data.deviceHandle);
      irradianceMap = textures.getAsset(asset.irradianceMap).data.deviceHandle;
      specularMap = textures.getAsset(asset.specularMap).data.deviceHandle;
    }

    if (entityDatabase.has<EnvironmentLightingSkyboxSource>(entity)) {
      if (environment.type == EnvironmentSkyboxType::Color) {
        frameData.setEnvironmentColor(environment.color);
      } else if (environment.type == EnvironmentSkyboxType::Texture &&
                 rhi::isHandleValid(irradianceMap)) {
        frameData.setEnvironmentTextures(irradianceMap, specularMap);
      }
    }
  }

  frameData.updateBuffers();
}

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline, bool bindMaterialData,
                           uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  uint32_t instanceStart = 0;

  for (auto &[handle, meshData] : frameData.getMeshGroups()) {
    const auto &mesh = mAssetRegistry.getMeshes().getAsset(handle).data;
    uint32_t numInstances = static_cast<uint32_t>(meshData.transforms.size());
    commandList.bindVertexBuffer(mesh.vertexBuffer.getHandle());
    commandList.bindIndexBuffer(mesh.indexBuffer.getHandle(),
                                rhi::IndexType::Uint32);

    int32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
    for (size_t g = 0; g < mesh.geometries.size(); ++g) {
      auto &geometry = mesh.geometries.at(g);

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 2,
                                   mesh.materials.at(g)->getDescriptor());
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      int32_t vertexCount =
          static_cast<int32_t>(mesh.geometries.at(g).vertices.size());

      commandList.drawIndexed(indexCount, indexOffset, vertexOffset,
                              numInstances, instanceStart);
      vertexOffset += vertexCount;
      indexOffset += indexCount;
    }
    instanceStart += numInstances;
  }
}

void SceneRenderer::renderSkinned(rhi::RenderCommandList &commandList,
                                  rhi::PipelineHandle pipeline,
                                  bool bindMaterialData, uint32_t frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  uint32_t instanceStart = 0;

  for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
    const auto &mesh = mAssetRegistry.getSkinnedMeshes().getAsset(handle).data;
    uint32_t numInstances = static_cast<uint32_t>(meshData.transforms.size());

    commandList.bindVertexBuffer(mesh.vertexBuffer.getHandle());
    commandList.bindIndexBuffer(mesh.indexBuffer.getHandle(),
                                rhi::IndexType::Uint32);

    int32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
    for (size_t g = 0; g < mesh.geometries.size(); ++g) {
      auto &geometry = mesh.geometries.at(g);

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 2,
                                   mesh.materials.at(g)->getDescriptor());
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      int32_t vertexCount =
          static_cast<int32_t>(mesh.geometries.at(g).vertices.size());

      commandList.drawIndexed(indexCount, indexOffset, vertexOffset,
                              numInstances, instanceStart);
      vertexOffset += vertexCount;
      indexOffset += indexCount;
    }
    instanceStart += numInstances;
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

    commandList.bindDescriptor(pipeline, 0,
                               mRenderStorage.getGlobalBuffersDescriptor());
    commandList.bindDescriptor(pipeline, 1,
                               mRenderStorage.getGlobalTexturesDescriptor());

    for (auto &text : texts) {
      frameData.getDrawParams().index9 = rhi::castHandleToUint(textureHandle);
      frameData.getDrawParams().index10 = text.glyphStart;

      commandList.pushConstants(
          pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
          sizeof(DrawParameters), &frameData.getDrawParams());

      commandList.draw(QuadNumVertices * static_cast<uint32_t>(text.length), 0,
                       1, text.index);
    }
  }
}

void SceneRenderer::generateBrdfLut() {
  static constexpr uint32_t GroupSize = 16;
  static constexpr uint32_t TextureSize = 512;

  rhi::DescriptorLayoutBindingDescription binding0{};
  binding0.type = rhi::DescriptorLayoutBindingType::Static;
  binding0.binding = 0;
  binding0.name = "uOutputTexture";
  binding0.shaderStage = rhi::ShaderStage::Compute;
  binding0.descriptorCount = 1;
  binding0.descriptorType = rhi::DescriptorType::StorageImage;

  auto layout = mDevice->createDescriptorLayout({{binding0}});

  auto pipeline = mDevice->createPipeline(rhi::ComputePipelineDescription{
      mShaderLibrary.getShader("__engine.pbr.brdfLut.compute")});

  rhi::TextureDescription textureDesc;
  textureDesc.type = rhi::TextureType::Standard;
  textureDesc.format = rhi::Format::Rgba16Float;
  textureDesc.height = TextureSize;
  textureDesc.width = TextureSize;
  textureDesc.layers = 1;
  textureDesc.levels = 1;
  textureDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled;

  auto brdfLut = mRenderStorage.createTexture(textureDesc);

  auto descriptor = mDevice->createDescriptor(layout);

  descriptor.write(0, {brdfLut}, rhi::DescriptorType::StorageImage);

  auto commandList = mDevice->requestImmediateCommandList();
  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(pipeline, 0, descriptor);
  commandList.dispatch(textureDesc.width / GroupSize,
                       textureDesc.height / GroupSize, 1);
  mDevice->submitImmediate(commandList);

  mDevice->destroyPipeline(pipeline);

  for (auto &frameData : mFrameData) {
    frameData.setBrdfLookupTable(brdfLut);
  }
}

} // namespace liquid
