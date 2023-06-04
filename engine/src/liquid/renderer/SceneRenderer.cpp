#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "SceneRenderer.h"
#include "StandardPushConstants.h"
#include "BindlessDrawParameters.h"

namespace liquid {

SceneRenderer::SceneRenderer(AssetRegistry &assetRegistry,
                             RenderStorage &renderStorage)
    : mAssetRegistry(assetRegistry), mRenderStorage(renderStorage),
      mFrameData{SceneRendererFrameData(renderStorage),
                 SceneRendererFrameData(renderStorage)} {

  auto shadersPath = Engine::getShadersPath();

  auto *device = mRenderStorage.getDevice();
  mMaxSampleCounts =
      device->getDeviceInformation().getLimits().framebufferColorSampleCounts &
      device->getDeviceInformation().getLimits().framebufferDepthSampleCounts;

  mMaxSampleCounts = std::max(1u, mMaxSampleCounts);

  static constexpr uint32_t MaxSamples = 8;
  for (uint32_t i = MaxSamples; i > 1; i = i / 2) {
    if ((mMaxSampleCounts & i) == i) {
      mMaxSampleCounts = i;
      break;
    }
  }

  mRenderStorage.createShader("__engine.sprite.default.vertex",
                              {shadersPath / "sprite.vert.spv"});
  mRenderStorage.createShader("__engine.sprite.default.fragment",
                              {shadersPath / "sprite.frag.spv"});

  mRenderStorage.createShader("__engine.geometry.default.vertex",
                              {shadersPath / "geometry.vert.spv"});
  mRenderStorage.createShader("__engine.geometry.skinned.vertex",
                              {shadersPath / "geometry-skinned.vert.spv"});
  mRenderStorage.createShader("__engine.pbr.default.fragment",
                              {shadersPath / "pbr.frag.spv"});
  mRenderStorage.createShader("__engine.skybox.default.vertex",
                              {shadersPath / "skybox.vert.spv"});
  mRenderStorage.createShader("__engine.skybox.default.fragment",
                              {shadersPath / "skybox.frag.spv"});
  mRenderStorage.createShader("__engine.shadowmap.default.vertex",
                              {shadersPath / "shadowmap.vert.spv"});
  mRenderStorage.createShader("__engine.shadowmap.skinned.vertex",
                              {shadersPath / "shadowmap-skinned.vert.spv"});

  mRenderStorage.createShader("__engine.shadowmap.default.fragment",
                              {shadersPath / "shadowmap.frag.spv"});

  mRenderStorage.createShader("__engine.text.default.vertex",
                              {shadersPath / "text.vert.spv"});

  mRenderStorage.createShader("__engine.text.default.fragment",
                              {shadersPath / "text.frag.spv"});

  mRenderStorage.createShader("__engine.pbr.brdfLut.compute",
                              {shadersPath / "generate-brdf-lut.comp.spv"});

  mRenderStorage.createShader("__engine.hdr.default.fragment",
                              {Engine::getShadersPath() / "hdr.frag.spv"});

  generateBrdfLut();
}

void SceneRenderer::setClearColor(const glm::vec4 &clearColor) {
  mClearColor = clearColor;
}

SceneRenderPassData SceneRenderer::attach(RenderGraph &graph) {
  static constexpr uint32_t ShadowMapDimensions = 4096;
  static constexpr uint32_t SwapchainSizePercentage = 100;

  rhi::TextureDescription shadowMapDesc{};
  shadowMapDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  shadowMapDesc.width = ShadowMapDimensions;
  shadowMapDesc.height = ShadowMapDimensions;
  shadowMapDesc.layerCount = SceneRendererFrameData::MaxShadowMaps;
  shadowMapDesc.format = rhi::Format::Depth16Unorm;
  shadowMapDesc.debugName = "Shadow maps";
  auto shadowmap = graph.create(shadowMapDesc)
                       .onReady([this](auto handle, RenderStorage &storage) {
                         for (auto &frameData : mFrameData) {
                           frameData.setShadowMapTexture(handle);
                         }
                         storage.addToDescriptor(handle);
                       });

  auto sceneColor = graph.create([this](auto width, auto height) {
    rhi::TextureDescription description{};
    description.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
    description.width = width;
    description.height = height;
    description.layerCount = 1;
    description.format = rhi::Format::Rgba16Float;
    description.samples = mMaxSampleCounts;
    description.debugName = "Sampled scene";

    return description;
  });

  auto sceneColorResolved =
      graph
          .create([this](auto width, auto height) {
            rhi::TextureDescription description{};
            description.usage =
                rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
            description.width = width;
            description.height = height;
            description.layerCount = 1;
            description.format = rhi::Format::Rgba16Float;
            description.samples = 1;
            description.debugName = "Resolved scene";

            return description;
          })
          .onReady([](rhi::TextureHandle handle, RenderStorage &storage) {
            storage.addToDescriptor(handle);
          });

  rhi::TextureDescription hdrColorDesc{};
  hdrColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  hdrColorDesc.width = SwapchainSizePercentage;
  hdrColorDesc.height = SwapchainSizePercentage;
  hdrColorDesc.layerCount = 1;
  hdrColorDesc.format = rhi::Format::Rgba8Srgb;
  hdrColorDesc.debugName = "HDR";

  auto hdrColorReal =
      mRenderStorage.createFramebufferRelativeTexture(hdrColorDesc);
  auto hdrColor = graph.import(hdrColorReal);

  auto depthBuffer = graph.create([this](auto width, auto height) {
    rhi::TextureDescription description{};
    description.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
    description.width = width;
    description.height = height;
    description.layerCount = 1;
    description.samples = mMaxSampleCounts;
    description.format = rhi::Format::Depth32Float;
    description.debugName = "Depth buffer";

    return description;
  });

  {
    struct ShadowDrawParams {
      rhi::DeviceAddress meshTransforms;
      rhi::DeviceAddress skinnedMeshTransforms;
      rhi::DeviceAddress skeletonTransforms;
      rhi::DeviceAddress shadows;
    };

    size_t shadowDrawOffset = 0;
    for (auto &frameData : mFrameData) {
      shadowDrawOffset =
          frameData.getBindlessParams().addRange(ShadowDrawParams{
              frameData.getMeshTransformsBuffer(),
              frameData.getSkinnedMeshTransformsBuffer(),
              frameData.getSkeletonsBuffer(), frameData.getShadowMapsBuffer()});
    }

    auto &pass = graph.addGraphicsPass("shadowPass");
    pass.write(shadowmap, AttachmentType::Depth,
               rhi::DepthStencilClear{1.0f, 0});

    auto pipeline = mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
        mRenderStorage.getShader("__engine.shadowmap.default.vertex"),
        mRenderStorage.getShader("__engine.shadowmap.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                rhi::FrontFace::Clockwise},
        {},
        {},
        {},
        "shadowmap mesh"});

    auto skinnedPipeline =
        mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
            mRenderStorage.getShader("__engine.shadowmap.skinned.vertex"),
            mRenderStorage.getShader("__engine.shadowmap.default.fragment"),
            rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
            rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
            rhi::PipelineRasterizer{rhi::PolygonMode::Fill,
                                    rhi::CullMode::Front,
                                    rhi::FrontFace::Clockwise},
            {},
            {},
            {},
            "shadowmap skinned mesh"});

    pass.addPipeline(pipeline);
    pass.addPipeline(skinnedPipeline);

    pass.setExecutor([pipeline, skinnedPipeline, shadowmap, shadowDrawOffset,
                      this](rhi::RenderCommandList &commandList,
                            uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      std::array<uint32_t, 1> offsets{static_cast<uint32_t>(shadowDrawOffset)};
      {
        LIQUID_PROFILE_EVENT("shadowPass::meshes");
        commandList.bindPipeline(pipeline);

        commandList.bindDescriptor(
            pipeline, 0, frameData.getBindlessParams().getDescriptor(),
            offsets);

        for (int32_t index = 0;
             index < static_cast<int32_t>(frameData.getNumShadowMaps());
             ++index) {
          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(uint32_t), &index);

          render(commandList, pipeline, false, frameIndex);
        }
      }

      {
        LIQUID_PROFILE_EVENT("shadowPass::skinnedMeshes");
        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(
            pipeline, 0, frameData.getBindlessParams().getDescriptor(),
            offsets);

        for (int32_t index = 0;
             index < static_cast<int32_t>(frameData.getNumShadowMaps());
             ++index) {

          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(uint32_t), &index);

          renderSkinned(commandList, skinnedPipeline, false, frameIndex);
        }
      }
    });
  } // shadow pass

  {
    struct MeshDrawParams {
      rhi::DeviceAddress meshTransforms;
      rhi::DeviceAddress skinnedMeshTransforms;
      rhi::DeviceAddress skeletonTransforms;
      rhi::DeviceAddress camera;
      rhi::DeviceAddress scene;
      rhi::DeviceAddress directionalLights;
      rhi::DeviceAddress pointLights;
      rhi::DeviceAddress shadows;
    };

    size_t pbrOffset = 0;
    for (auto &frameData : mFrameData) {
      pbrOffset = frameData.getBindlessParams().addRange(MeshDrawParams{
          frameData.getMeshTransformsBuffer(),
          frameData.getSkinnedMeshTransformsBuffer(),
          frameData.getSkeletonsBuffer(), frameData.getCameraBuffer(),
          frameData.getSceneBuffer(), frameData.getDirectionalLightsBuffer(),
          frameData.getPointLightsBuffer(), frameData.getShadowMapsBuffer()});
    }

    auto &pass = graph.addGraphicsPass("meshPass");
    pass.read(shadowmap);
    pass.write(sceneColor, AttachmentType::Color, mClearColor);
    pass.write(depthBuffer, AttachmentType::Depth,
               rhi::DepthStencilClear{1.0, 0});
    pass.write(sceneColorResolved, AttachmentType::Resolve, mClearColor);

    auto pipeline = mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
        mRenderStorage.getShader("__engine.geometry.default.vertex"),
        mRenderStorage.getShader("__engine.pbr.default.fragment"),
        rhi::PipelineVertexInputLayout::create<Vertex>(),
        rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
        rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                rhi::FrontFace::Clockwise},
        rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
        {},
        rhi::PipelineMultisample{0},
        "mesh"});

    auto skinnedPipeline =
        mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
            mRenderStorage.getShader("__engine.geometry.skinned.vertex"),
            mRenderStorage.getShader("__engine.pbr.default.fragment"),
            rhi::PipelineVertexInputLayout::create<SkinnedVertex>(),
            rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
            rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                    rhi::FrontFace::Clockwise},
            rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
            {},
            rhi::PipelineMultisample{0},
            "skinned mesh"});

    pass.addPipeline(pipeline);
    pass.addPipeline(skinnedPipeline);

    pass.setExecutor([this, pipeline, skinnedPipeline, pbrOffset,
                      shadowmap](rhi::RenderCommandList &commandList,
                                 uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      commandList.bindPipeline(pipeline);

      std::array<uint32_t, 1> offsets{static_cast<uint32_t>(pbrOffset)};
      {
        LIQUID_PROFILE_EVENT("meshPass::meshes");

        commandList.bindPipeline(pipeline);
        commandList.bindDescriptor(
            pipeline, 0, mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindDescriptor(
            pipeline, 1, frameData.getBindlessParams().getDescriptor(),
            offsets);

        render(commandList, pipeline, true, frameIndex);
      }

      {
        LIQUID_PROFILE_EVENT("meshPass::skinnedMeshes");

        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(
            skinnedPipeline, 0, mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindDescriptor(
            skinnedPipeline, 1, frameData.getBindlessParams().getDescriptor(),
            offsets);

        renderSkinned(commandList, skinnedPipeline, true, frameIndex);
      }
    });
  } // mesh pass

  {
    struct SpriteDrawParams {
      rhi::DeviceAddress camera;
      rhi::DeviceAddress transforms;
      rhi::DeviceAddress textures;
      rhi::DeviceAddress pad0;
    };

    auto &pass = graph.addGraphicsPass("spritePass");
    pass.write(sceneColor, AttachmentType::Color, mClearColor);
    pass.write(depthBuffer, AttachmentType::Depth,
               rhi::DepthStencilClear{1.0f, 0});
    pass.write(sceneColorResolved, AttachmentType::Resolve, mClearColor);

    auto pipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("__engine.sprite.default.vertex"),
         mRenderStorage.getShader("__engine.sprite.default.fragment"),
         {},
         rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleStrip},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
         {},
         {},
         "sprite"});

    pass.addPipeline(pipeline);

    size_t spriteOffset = 0;
    for (auto &frameData : mFrameData) {
      spriteOffset = frameData.getBindlessParams().addRange(SpriteDrawParams{
          frameData.getCameraBuffer(), frameData.getSpriteTransformsBuffer(),
          frameData.getSpriteTexturesBuffer(), frameData.getCameraBuffer()});
    }

    pass.setExecutor([pipeline, spriteOffset,
                      this](rhi::RenderCommandList &commandList,
                            uint32_t frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      std::array<uint32_t, 1> offsets{static_cast<uint32_t>(spriteOffset)};

      commandList.bindPipeline(pipeline);
      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalTexturesDescriptor());
      commandList.bindDescriptor(
          pipeline, 1, frameData.getBindlessParams().getDescriptor(), offsets);

      commandList.draw(
          4, 0, static_cast<uint32_t>(frameData.getSpriteEntities().size()), 0);
    });
  } // sprite pass

  {
    struct SkyboxDrawParams {
      rhi::DeviceAddress camera;
      rhi::DeviceAddress skybox;
    };

    size_t skyboxOffset = 0;
    for (auto &frameData : mFrameData) {
      skyboxOffset = frameData.getBindlessParams().addRange(SkyboxDrawParams{
          frameData.getCameraBuffer(), frameData.getSkyboxBuffer()});
    }

    auto &pass = graph.addGraphicsPass("skyboxPass");
    pass.write(sceneColor, AttachmentType::Color, mClearColor);
    pass.write(depthBuffer, AttachmentType::Depth,
               rhi::DepthStencilClear{1.0f, 0});
    pass.write(sceneColorResolved, AttachmentType::Resolve, mClearColor);

    auto pipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("__engine.skybox.default.vertex"),
         mRenderStorage.getShader("__engine.skybox.default.fragment"),
         rhi::PipelineVertexInputLayout::create<Vertex>(),
         rhi::PipelineInputAssembly{},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
         {},
         {},
         "skybox"});

    pass.addPipeline(pipeline);

    pass.setExecutor([pipeline, skyboxOffset,
                      this](rhi::RenderCommandList &commandList,
                            uint32_t frameIndex) {
      std::array<uint32_t, 1> offsets{static_cast<uint32_t>(skyboxOffset)};

      auto &frameData = mFrameData.at(frameIndex);

      commandList.bindPipeline(pipeline);

      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalTexturesDescriptor());
      commandList.bindDescriptor(
          pipeline, 1, frameData.getBindlessParams().getDescriptor(), offsets);

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

  {
    auto &pass = graph.addGraphicsPass("hdrPass");
    pass.read(sceneColorResolved);
    pass.write(hdrColor, AttachmentType::Color, mClearColor);

    rhi::GraphicsPipelineDescription pipelineDescription{};
    pipelineDescription.vertexShader =
        mRenderStorage.getShader("__engine.fullscreenQuad.default.vertex");
    pipelineDescription.fragmentShader =
        mRenderStorage.getShader("__engine.hdr.default.fragment");
    pipelineDescription.rasterizer = rhi::PipelineRasterizer{
        liquid::rhi::PolygonMode::Fill, liquid::rhi::CullMode::Front,
        liquid::rhi::FrontFace::CounterClockwise};
    pipelineDescription.colorBlend.attachments = {
        liquid::rhi::PipelineColorBlendAttachment{}};
    pipelineDescription.debugName = "hdr";

    auto pipeline = mRenderStorage.addPipeline(pipelineDescription);
    pass.addPipeline(pipeline);

    pass.setExecutor([pipeline, sceneColorResolved,
                      this](rhi::RenderCommandList &commandList,
                            uint32_t frameIndex) {
      commandList.bindPipeline(pipeline);
      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalTexturesDescriptor());

      uint32_t color = static_cast<uint32_t>(sceneColorResolved.getHandle());

      commandList.pushConstants(pipeline, rhi::ShaderStage::Fragment, 0,
                                sizeof(uint32_t), &color);
      commandList.draw(3, 0);
    });
  }

  LOG_DEBUG("Scene renderer attached to graph");

  for (auto &frameData : mFrameData) {
    frameData.getBindlessParams().build(mRenderStorage.getDevice());
  }

  return SceneRenderPassData{sceneColor, sceneColorResolved, hdrColor,
                             depthBuffer, mMaxSampleCounts};
}

void SceneRenderer::attachText(RenderGraph &graph,
                               const SceneRenderPassData &passData) {
  struct TextDrawParams {
    rhi::DeviceAddress textTransforms;
    rhi::DeviceAddress camera;
    rhi::DeviceAddress glyphs;
    rhi::DeviceAddress pad0;
  };

  size_t textOffset = 0;
  for (auto &frameData : mFrameData) {
    textOffset = frameData.getBindlessParams().addRange(TextDrawParams{
        frameData.getTextTransformsBuffer(), frameData.getCameraBuffer(),
        frameData.getGlyphsBuffer()});
  }

  auto &pass = graph.addGraphicsPass("textPass");
  pass.write(passData.sceneColor, AttachmentType::Color, mClearColor);
  pass.write(passData.depthBuffer, AttachmentType::Depth,
             rhi::DepthStencilClear{1.0f, 0});
  pass.write(passData.sceneColorResolved, AttachmentType::Resolve, mClearColor);

  auto textPipeline =
      mRenderStorage.addPipeline(rhi::GraphicsPipelineDescription{
          mRenderStorage.getShader("__engine.text.default.vertex"),
          mRenderStorage.getShader("__engine.text.default.fragment"),
          rhi::PipelineVertexInputLayout{},
          rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
          rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                  rhi::FrontFace::Clockwise},
          rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{
              true, rhi::BlendFactor::SrcAlpha,
              rhi::BlendFactor::OneMinusSrcAlpha, rhi::BlendOp::Add,
              rhi::BlendFactor::One, rhi::BlendFactor::OneMinusSrcAlpha,
              rhi::BlendOp::Add}}},
          {},
          {},
          "text"});

  pass.addPipeline(textPipeline);

  pass.setExecutor([textPipeline, textOffset,
                    this](rhi::RenderCommandList &commandList,
                          uint32_t frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    std::array<uint32_t, 1> offsets{static_cast<uint32_t>(textOffset)};
    commandList.bindPipeline(textPipeline);
    commandList.bindDescriptor(textPipeline, 1,
                               frameData.getBindlessParams().getDescriptor(),
                               offsets);
    renderText(commandList, textPipeline, frameIndex);
  });

  for (auto &frameData : mFrameData) {
    frameData.getBindlessParams().build(mRenderStorage.getDevice());
  }
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

  for (auto [entity, sprite, world] :
       entityDatabase.view<Sprite, WorldTransform>()) {
    auto handle =
        mAssetRegistry.getTextures().getAsset(sprite.handle).data.deviceHandle;
    frameData.addSprite(entity, handle, world.worldTransform);
  }

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

  // Directional lights
  for (auto [entity, light] : entityDatabase.view<DirectionalLight>()) {
    if (entityDatabase.has<CascadedShadowMap>(entity)) {
      frameData.addLight(light, entityDatabase.get<CascadedShadowMap>(entity));
    } else {
      frameData.addLight(light);
    }
  };

  // Point lights
  for (auto [entity, light, world] :
       entityDatabase.view<PointLight, WorldTransform>()) {
    frameData.addLight(light, world);
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
                               mRenderStorage.getGlobalTexturesDescriptor());

    for (auto &text : texts) {
      glm::uvec4 textConstants{rhi::castHandleToUint(textureHandle),
                               text.glyphStart, 0, 0};

      commandList.pushConstants(
          pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
          sizeof(glm::uvec4), glm::value_ptr(textConstants));

      commandList.draw(QuadNumVertices * static_cast<uint32_t>(text.length), 0,
                       1, text.index);
    }
  }
}

void SceneRenderer::generateBrdfLut() {
  auto *device = mRenderStorage.getDevice();

  static constexpr uint32_t GroupSize = 16;
  static constexpr uint32_t TextureSize = 512;

  rhi::DescriptorLayoutBindingDescription binding0{};
  binding0.type = rhi::DescriptorLayoutBindingType::Static;
  binding0.binding = 0;
  binding0.name = "uOutputTexture";
  binding0.shaderStage = rhi::ShaderStage::Compute;
  binding0.descriptorCount = 1;
  binding0.descriptorType = rhi::DescriptorType::StorageImage;

  auto layout = device->createDescriptorLayout({{binding0}});

  auto pipeline = mRenderStorage.addPipeline(rhi::ComputePipelineDescription(
      {mRenderStorage.getShader("__engine.pbr.brdfLut.compute")}));

  device->createPipeline(mRenderStorage.getComputePipelineDescription(pipeline),
                         pipeline);

  rhi::TextureDescription textureDesc;
  textureDesc.type = rhi::TextureType::Standard;
  textureDesc.format = rhi::Format::Rgba16Float;
  textureDesc.height = TextureSize;
  textureDesc.width = TextureSize;
  textureDesc.layerCount = 1;
  textureDesc.mipLevelCount = 1;
  textureDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled;
  textureDesc.debugName = "BRDF LUT";

  auto brdfLut = mRenderStorage.createTexture(textureDesc);

  auto descriptor = device->createDescriptor(layout);

  std::array<rhi::TextureHandle, 1> textures{brdfLut};
  descriptor.write(0, textures, rhi::DescriptorType::StorageImage);

  auto commandList = device->requestImmediateCommandList();
  commandList.bindPipeline(pipeline);
  commandList.bindDescriptor(pipeline, 0, descriptor);
  commandList.dispatch(textureDesc.width / GroupSize,
                       textureDesc.height / GroupSize, 1);
  device->submitImmediate(commandList);

  device->destroyPipeline(pipeline);

  for (auto &frameData : mFrameData) {
    frameData.setBrdfLookupTable(brdfLut);
  }
}

} // namespace liquid
