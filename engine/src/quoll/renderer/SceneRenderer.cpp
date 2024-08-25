#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Profiler.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/Sprite.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/text/Text.h"
#include "BindlessDrawParameters.h"
#include "Mesh.h"
#include "MeshRenderUtils.h"
#include "MeshVertexLayout.h"
#include "RenderStorage.h"
#include "SceneRenderer.h"
#include "SkinnedMesh.h"
#include "StandardPushConstants.h"

namespace quoll {

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

  static constexpr u32 MaxSamples = 8;
  for (u32 i = MaxSamples; i > 1; i = i / 2) {
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
  mRenderStorage.createShader(
      "__engine.bloom.extract-bright-colors.compute",
      {Engine::getShadersPath() / "extract-bright-colors.comp.spv"});
  mRenderStorage.createShader(
      "__engine.bloom.downsample.compute",
      {Engine::getShadersPath() / "bloom-downsample.comp.spv"});
  mRenderStorage.createShader(
      "__engine.bloom.upsample.compute",
      {Engine::getShadersPath() / "bloom-upsample.comp.spv"});

  generateBrdfLut();

  {
    rhi::SamplerDescription description{};
    description.wrapModeU = rhi::WrapMode::ClampToEdge;
    description.wrapModeV = rhi::WrapMode::ClampToEdge;
    description.wrapModeW = rhi::WrapMode::ClampToEdge;
    description.debugName = "bloom";
    mBloomSampler = mRenderStorage.createSampler(description);
  }
}

void SceneRenderer::setClearColor(const glm::vec4 &clearColor) {
  mClearColor = clearColor;
}

SceneRenderPassData SceneRenderer::attach(RenderGraph &graph,
                                          const RendererOptions &options) {
  for (auto &frameData : mFrameData) {
    frameData.getBindlessParams().destroy(mRenderStorage.getDevice());
  }

  static constexpr u32 ShadowMapDimensions = 4096;

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

  rhi::TextureDescription sceneColorDesc{};
  sceneColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  sceneColorDesc.width = options.framebufferSize.x;
  sceneColorDesc.height = options.framebufferSize.y;
  sceneColorDesc.layerCount = 1;
  sceneColorDesc.format = rhi::Format::Rgba16Float;
  sceneColorDesc.samples = mMaxSampleCounts;
  sceneColorDesc.debugName = "Scene";

  auto sceneColor = graph.create(sceneColorDesc);

  auto sceneColorResolvedDesc = sceneColorDesc;
  sceneColorResolvedDesc.samples = 1;
  sceneColorResolvedDesc.debugName = "Resolved scene";
  auto sceneColorResolved =
      graph.create(sceneColorResolvedDesc)
          .onReady([](rhi::TextureHandle handle, RenderStorage &storage) {
            storage.addToDescriptor(handle);
          });

  rhi::TextureDescription hdrColorDesc{};
  hdrColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  hdrColorDesc.width = options.framebufferSize.x;
  hdrColorDesc.height = options.framebufferSize.y;
  hdrColorDesc.layerCount = 1;
  hdrColorDesc.format = rhi::Format::Rgba8Srgb;
  hdrColorDesc.debugName = "HDR";

  auto hdrColor =
      graph.create(hdrColorDesc)
          .onReady([](rhi::TextureHandle handle, RenderStorage &storage) {
            storage.addToDescriptor(handle);
          });

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = options.framebufferSize.x;
  depthBufferDesc.height = options.framebufferSize.y;
  depthBufferDesc.layerCount = 1;
  depthBufferDesc.samples = mMaxSampleCounts;
  depthBufferDesc.format = rhi::Format::Depth32Float;
  depthBufferDesc.debugName = "Depth buffer";

  auto depthBuffer = graph.create(depthBufferDesc);

  {
    struct ShadowDrawParams {
      rhi::DeviceAddress meshTransforms;
      rhi::DeviceAddress skinnedMeshTransforms;
      rhi::DeviceAddress skeletonTransforms;
      rhi::DeviceAddress shadows;
    };

    usize shadowDrawOffset = 0;
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
        createMeshPositionLayout(),
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
            createSkinnedMeshPositionLayout(),
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
                            u32 frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      std::array<u32, 1> offsets{static_cast<u32>(shadowDrawOffset)};
      {
        QUOLL_PROFILE_EVENT("shadowPass::meshes");
        commandList.bindPipeline(pipeline);

        commandList.bindDescriptor(
            pipeline, 0, frameData.getBindlessParams().getDescriptor(),
            offsets);

        for (i32 index = 0;
             index < static_cast<i32>(frameData.getNumShadowMaps()); ++index) {
          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(u32), &index);

          renderShadowsMesh(commandList, pipeline, frameIndex);
        }
      }

      {
        QUOLL_PROFILE_EVENT("shadowPass::skinnedMeshes");
        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(
            pipeline, 0, frameData.getBindlessParams().getDescriptor(),
            offsets);

        for (i32 index = 0;
             index < static_cast<i32>(frameData.getNumShadowMaps()); ++index) {

          commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                                    sizeof(u32), &index);

          renderShadowsSkinnedMesh(commandList, skinnedPipeline, frameIndex);
        }
      }
    });
  } // shadow pass

  {
    struct MeshDrawParams {
      rhi::DeviceAddress materials;
      rhi::DeviceAddress meshTransforms;
      rhi::DeviceAddress meshMaterials;
      rhi::DeviceAddress skinnedMeshTransforms;
      rhi::DeviceAddress skinnedMeshMaterials;
      rhi::DeviceAddress skeletonTransforms;
      rhi::DeviceAddress camera;
      rhi::DeviceAddress scene;
      rhi::DeviceAddress directionalLights;
      rhi::DeviceAddress pointLights;
      rhi::DeviceAddress shadows;
      rhi::SamplerHandle sampler;
    };

    usize pbrOffset = 0;
    for (auto &frameData : mFrameData) {
      pbrOffset = frameData.getBindlessParams().addRange(MeshDrawParams{
          frameData.getFlattenedMaterialsBuffer(),
          frameData.getMeshTransformsBuffer(),
          frameData.getMeshMaterialsBuffer(),
          frameData.getSkinnedMeshTransformsBuffer(),
          frameData.getSkinnedMeshMaterialsBuffer(),
          frameData.getSkeletonsBuffer(), frameData.getCameraBuffer(),
          frameData.getSceneBuffer(), frameData.getDirectionalLightsBuffer(),
          frameData.getPointLightsBuffer(), frameData.getShadowMapsBuffer(),
          mRenderStorage.getDefaultSampler()});
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
        createMeshVertexLayout(),
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
            createSkinnedMeshVertexLayout(),
            rhi::PipelineInputAssembly{rhi::PrimitiveTopology::TriangleList},
            rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::None,
                                    rhi::FrontFace::Clockwise},
            rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
            {},
            rhi::PipelineMultisample{0},
            "skinned mesh"});

    pass.addPipeline(pipeline);
    pass.addPipeline(skinnedPipeline);

    pass.setExecutor([this, pipeline, skinnedPipeline, pbrOffset, shadowmap](
                         rhi::RenderCommandList &commandList, u32 frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      commandList.bindPipeline(pipeline);

      std::array<u32, 1> offsets{static_cast<u32>(pbrOffset)};
      {
        QUOLL_PROFILE_EVENT("meshPass::meshes");

        commandList.bindPipeline(pipeline);
        commandList.bindDescriptor(
            pipeline, 0, mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindDescriptor(
            pipeline, 1, frameData.getBindlessParams().getDescriptor(),
            offsets);

        render(commandList, pipeline, frameIndex);
      }

      {
        QUOLL_PROFILE_EVENT("meshPass::skinnedMeshes");

        commandList.bindPipeline(skinnedPipeline);
        commandList.bindDescriptor(
            skinnedPipeline, 0, mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindDescriptor(
            skinnedPipeline, 1, frameData.getBindlessParams().getDescriptor(),
            offsets);

        renderSkinned(commandList, skinnedPipeline, frameIndex);
      }
    });
  } // mesh pass

  {
    struct SpriteDrawParams {
      rhi::DeviceAddress camera;
      rhi::DeviceAddress transforms;
      rhi::DeviceAddress textures;
      rhi::SamplerHandle sampler;
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

    usize spriteOffset = 0;
    for (auto &frameData : mFrameData) {
      spriteOffset = frameData.getBindlessParams().addRange(SpriteDrawParams{
          frameData.getCameraBuffer(), frameData.getSpriteTransformsBuffer(),
          frameData.getSpriteTexturesBuffer(),
          mRenderStorage.getDefaultSampler()});
    }

    pass.setExecutor([pipeline, spriteOffset, this](
                         rhi::RenderCommandList &commandList, u32 frameIndex) {
      auto &frameData = mFrameData.at(frameIndex);

      std::array<u32, 1> offsets{static_cast<u32>(spriteOffset)};

      commandList.bindPipeline(pipeline);
      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalTexturesDescriptor());
      commandList.bindDescriptor(
          pipeline, 1, frameData.getBindlessParams().getDescriptor(), offsets);

      commandList.draw(
          4, 0, static_cast<u32>(frameData.getSpriteEntities().size()), 0);
    });
  } // sprite pass

  {
    struct SkyboxDrawParams {
      rhi::DeviceAddress camera;
      rhi::DeviceAddress skybox;
      rhi::SamplerHandle defaultSampler;
    };

    usize skyboxOffset = 0;
    for (auto &frameData : mFrameData) {
      skyboxOffset = frameData.getBindlessParams().addRange(SkyboxDrawParams{
          frameData.getCameraBuffer(), frameData.getSkyboxBuffer(),
          mRenderStorage.getDefaultSampler()});
    }

    auto &pass = graph.addGraphicsPass("skyboxPass");
    pass.write(sceneColor, AttachmentType::Color, mClearColor);
    pass.write(depthBuffer, AttachmentType::Depth,
               rhi::DepthStencilClear{1.0f, 0});
    pass.write(sceneColorResolved, AttachmentType::Resolve, mClearColor);

    auto pipeline = mRenderStorage.addPipeline(
        {mRenderStorage.getShader("__engine.skybox.default.vertex"),
         mRenderStorage.getShader("__engine.skybox.default.fragment"),
         createMeshPositionLayout(),
         rhi::PipelineInputAssembly{},
         rhi::PipelineRasterizer{rhi::PolygonMode::Fill, rhi::CullMode::Front,
                                 rhi::FrontFace::Clockwise},
         rhi::PipelineColorBlend{{rhi::PipelineColorBlendAttachment{}}},
         {},
         {},
         "skybox"});

    pass.addPipeline(pipeline);

    pass.setExecutor([pipeline, skyboxOffset, this](
                         rhi::RenderCommandList &commandList, u32 frameIndex) {
      std::array<u32, 1> offsets{static_cast<u32>(skyboxOffset)};

      auto &frameData = mFrameData.at(frameIndex);

      commandList.bindPipeline(pipeline);

      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalTexturesDescriptor());
      commandList.bindDescriptor(
          pipeline, 1, frameData.getBindlessParams().getDescriptor(), offsets);

      const auto &cube =
          mAssetRegistry.get(mAssetRegistry.getDefaultObjects().cube).data;

      std::array<u64, 1> vbOffsets{0};
      commandList.bindVertexBuffers(std::array{cube.vertexBuffers.at(0)},
                                    vbOffsets);
      commandList.bindIndexBuffer(cube.indexBuffer, rhi::IndexType::Uint32);

      commandList.drawIndexed(
          static_cast<u32>(cube.geometries.at(0).indices.size()), 0, 0);
    });
  } // skybox pass

  static constexpr u32 BloomMipChainSize = 7;

  rhi::TextureDescription description{};
  description.debugName = "Bloom";
  description.mipLevelCount = BloomMipChainSize;
  description.format = rhi::Format::Rgba16Float;
  description.usage = rhi::TextureUsage::Sampled | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Color;
  description.width = options.framebufferSize.x;
  description.height = options.framebufferSize.y;

  auto bloomTexture =
      graph.create(description).onReady([](auto handle, auto &storage) {
        storage.addToDescriptor(handle);
      });

  {
    auto &pass = graph.addComputePass("bloom");
    pass.read(sceneColorResolved);
    pass.write(bloomTexture, AttachmentType::Color, mClearColor);

    struct BloomMip {
      RenderGraphResource<rhi::TextureHandle> texture;
      glm::uvec2 size;
    };

    std::vector<BloomMip> bloomChain;
    bloomChain.push_back({bloomTexture, options.framebufferSize});
    bloomChain.reserve(BloomMipChainSize);
    for (u32 i = 1; i < BloomMipChainSize; ++i) {
      auto view = graph.createView(bloomTexture, i)
                      .onReady([](auto handle, auto &storage) {
                        storage.addToDescriptor(handle);
                      });
      bloomChain.push_back({view, options.framebufferSize / 2u});
    }

    auto extractBrightColorsPipeline =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{
            mRenderStorage.getShader(
                "__engine.bloom.extract-bright-colors.compute"),
            "extract bright colors"});
    pass.addPipeline(extractBrightColorsPipeline);

    auto downsamplePipeline =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{
            mRenderStorage.getShader("__engine.bloom.downsample.compute"),
            "bloom downsample"});
    pass.addPipeline(downsamplePipeline);

    auto upsamplePipeline =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{
            mRenderStorage.getShader("__engine.bloom.upsample.compute"),
            "bloom upsample"});
    pass.addPipeline(upsamplePipeline);

    static constexpr u32 WorkGroupSize = 32;

    pass.setExecutor([extractBrightColorsPipeline, downsamplePipeline,
                      upsamplePipeline, bloomTexture, sceneColorResolved,
                      &options, bloomChain, this](
                         rhi::RenderCommandList &commandList, u32 frameIndex) {
      // Extract bright colors
      {
        commandList.bindDescriptor(
            extractBrightColorsPipeline, 0,
            mRenderStorage.getGlobalTexturesDescriptor());

        glm::uvec4 texture{static_cast<u32>(sceneColorResolved.getHandle()),
                           static_cast<u32>(bloomTexture.getHandle()),
                           static_cast<u32>(mBloomSampler), 0};
        commandList.pushConstants(extractBrightColorsPipeline,
                                  rhi::ShaderStage::Compute, 0,
                                  sizeof(glm::uvec4), glm::value_ptr(texture));

        commandList.bindPipeline(extractBrightColorsPipeline);
        commandList.dispatch(options.framebufferSize.x / WorkGroupSize,
                             options.framebufferSize.y / WorkGroupSize, 1);
      }

      // Downsample
      {
        commandList.bindDescriptor(
            downsamplePipeline, 0,
            mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindPipeline(downsamplePipeline);

        for (u32 level = 1; level < static_cast<u32>(bloomChain.size());
             ++level) {

          const auto &source = bloomChain.at(level - 1);
          const auto &target = bloomChain.at(level);

          rhi::ImageBarrier imageBarrier{};
          imageBarrier.baseLevel = level - 1;
          imageBarrier.levelCount = 1;
          imageBarrier.srcAccess = rhi::Access::ShaderWrite;
          imageBarrier.srcLayout = rhi::ImageLayout::General;
          imageBarrier.dstAccess = rhi::Access::ShaderRead;
          imageBarrier.dstLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
          imageBarrier.texture = bloomTexture;
          imageBarrier.srcStage = rhi::PipelineStage::ComputeShader;
          imageBarrier.dstStage = rhi::PipelineStage::ComputeShader;

          std::array<rhi::ImageBarrier, 1> imageBarriers{imageBarrier};

          commandList.pipelineBarrier({}, imageBarriers, {});

          glm::uvec4 texture{static_cast<u32>(source.texture.getHandle()),
                             static_cast<u32>(target.texture.getHandle()),
                             static_cast<u32>(mBloomSampler), level - 1};
          commandList.pushConstants(
              downsamplePipeline, rhi::ShaderStage::Compute, 0,
              sizeof(glm::uvec4), glm::value_ptr(texture));
          commandList.dispatch(target.size.x / WorkGroupSize,
                               target.size.y / WorkGroupSize, 1);
        }
      }

      // Upsample
      {
        commandList.bindDescriptor(
            upsamplePipeline, 0, mRenderStorage.getGlobalTexturesDescriptor());
        commandList.bindPipeline(upsamplePipeline);
        for (u32 level = static_cast<u32>(bloomChain.size() - 1); level > 0;
             --level) {

          rhi::ImageBarrier imageBarrierSrc{};
          imageBarrierSrc.baseLevel = level;
          imageBarrierSrc.levelCount = 1;
          imageBarrierSrc.srcAccess = rhi::Access::ShaderWrite;
          imageBarrierSrc.srcLayout = rhi::ImageLayout::General;
          imageBarrierSrc.dstAccess = rhi::Access::ShaderRead;
          imageBarrierSrc.dstLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
          imageBarrierSrc.srcStage = rhi::PipelineStage::ComputeShader;
          imageBarrierSrc.dstStage = rhi::PipelineStage::ComputeShader;
          imageBarrierSrc.texture = bloomTexture;

          rhi::ImageBarrier imageBarrierDst{};
          imageBarrierDst.baseLevel = level - 1;
          imageBarrierDst.levelCount = 1;
          imageBarrierDst.srcAccess = rhi::Access::ShaderRead;
          imageBarrierDst.srcLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
          imageBarrierDst.dstAccess = rhi::Access::ShaderWrite;
          imageBarrierDst.dstLayout = rhi::ImageLayout::General;
          imageBarrierDst.texture = bloomTexture;
          imageBarrierDst.srcStage = rhi::PipelineStage::ComputeShader;
          imageBarrierDst.dstStage = rhi::PipelineStage::ComputeShader;

          std::array<rhi::ImageBarrier, 2> imageBarriers{imageBarrierSrc,
                                                         imageBarrierDst};

          commandList.pipelineBarrier({}, imageBarriers, {});

          const auto &source = bloomChain.at(level);
          const auto &target = bloomChain.at(level - 1);

          glm::uvec4 texture{static_cast<u32>(source.texture.getHandle()),
                             static_cast<u32>(target.texture.getHandle()),
                             static_cast<u32>(mBloomSampler), 0};
          commandList.pushConstants(upsamplePipeline, rhi::ShaderStage::Compute,
                                    0, sizeof(glm::uvec4),
                                    glm::value_ptr(texture));
          commandList.dispatch(target.size.x / WorkGroupSize,
                               target.size.y / WorkGroupSize, 1);
        }
      }
    });
  }

  {
    auto &pass = graph.addGraphicsPass("hdrPass");
    pass.read(sceneColorResolved);
    pass.read(bloomTexture);
    pass.write(hdrColor, AttachmentType::Color, mClearColor);

    rhi::GraphicsPipelineDescription pipelineDescription{};
    pipelineDescription.vertexShader =
        mRenderStorage.getShader("__engine.fullscreenQuad.default.vertex");
    pipelineDescription.fragmentShader =
        mRenderStorage.getShader("__engine.hdr.default.fragment");
    pipelineDescription.rasterizer = rhi::PipelineRasterizer{
        quoll::rhi::PolygonMode::Fill, quoll::rhi::CullMode::Front,
        quoll::rhi::FrontFace::CounterClockwise};
    pipelineDescription.colorBlend.attachments = {
        quoll::rhi::PipelineColorBlendAttachment{}};
    pipelineDescription.debugName = "hdr";

    auto pipeline = mRenderStorage.addPipeline(pipelineDescription);
    pass.addPipeline(pipeline);

    pass.setExecutor([pipeline, sceneColorResolved, bloomTexture, this](
                         rhi::RenderCommandList &commandList, u32 frameIndex) {
      commandList.bindPipeline(pipeline);
      commandList.bindDescriptor(pipeline, 0,
                                 mRenderStorage.getGlobalTexturesDescriptor());

      struct Data {
        rhi::DeviceAddress bufferAddress;

        rhi::TextureHandle sceneColor;

        rhi::TextureHandle bloomTexture;

        rhi::SamplerHandle defaultSampler;
      };

      Data data{
          mFrameData.at(frameIndex).getCameraBuffer(),
          sceneColorResolved.getHandle(),
          bloomTexture.getHandle(),
          mRenderStorage.getDefaultSampler(),
      };

      commandList.pushConstants(pipeline, rhi::ShaderStage::Fragment, 0,
                                sizeof(Data), &data);

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

  usize textOffset = 0;
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
                    this](rhi::RenderCommandList &commandList, u32 frameIndex) {
    auto &frameData = mFrameData.at(frameIndex);

    std::array<u32, 1> offsets{static_cast<u32>(textOffset)};
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
                                    Entity camera, u32 frameIndex) {
  QuollAssert(entityDatabase.has<Camera>(camera),
              "Entity does not have a camera");

  auto &frameData = mFrameData.at(frameIndex);

  QUOLL_PROFILE_EVENT("SceneRenderer::updateFrameData");
  frameData.clear();

  frameData.setCameraData(entityDatabase.get<Camera>(camera),
                          entityDatabase.get<PerspectiveLens>(camera));

  frameData.setDefaultMaterial(
      mAssetRegistry.get(mAssetRegistry.getDefaultObjects().defaultMaterial)
          .data.deviceHandle->getAddress());

  for (auto [entity, sprite, world] :
       entityDatabase.view<Sprite, WorldTransform>()) {
    auto handle = mAssetRegistry.get(sprite.handle).data.deviceHandle;
    frameData.addSprite(entity, handle, world.worldTransform);
  }

  // Meshes
  for (auto [entity, world, mesh, renderer] :
       entityDatabase.view<WorldTransform, Mesh, MeshRenderer>()) {
    const auto &asset = mAssetRegistry.get(mesh.handle);

    std::vector<rhi::DeviceAddress> materials;
    for (auto material : renderer.materials) {
      materials.push_back(
          mAssetRegistry.get(material).data.deviceHandle->getAddress());
    }

    frameData.addMesh(mesh.handle, entity, world.worldTransform, materials);
  }

  // Skinned Meshes
  for (auto [entity, skeleton, world, mesh, renderer] :
       entityDatabase.view<Skeleton, WorldTransform, SkinnedMesh,
                           SkinnedMeshRenderer>()) {
    const auto &asset = mAssetRegistry.get(mesh.handle);

    std::vector<rhi::DeviceAddress> materials;
    for (auto material : renderer.materials) {
      materials.push_back(
          mAssetRegistry.get(material).data.deviceHandle->getAddress());
    }

    frameData.addSkinnedMesh(mesh.handle, entity, world.worldTransform,
                             skeleton.jointFinalTransforms, materials);
  }

  // Texts
  for (auto [entity, text, world] :
       entityDatabase.view<Text, WorldTransform>()) {
    const auto &font = mAssetRegistry.get(text.font).data;

    std::vector<SceneRendererFrameData::GlyphData> glyphs(
        text.content.length());
    f32 advanceX = 0;
    f32 advanceY = 0;
    for (usize i = 0; i < text.content.length(); ++i) {
      char c = text.content.at(i);

      if (c == '\n') {
        advanceX = 0.0f;
        advanceY += text.lineHeight * font.fontScale;
        continue;
      }

      const auto &fontGlyph = font.glyphs.at(c);
      glyphs.at(i).atlasBounds = fontGlyph.atlasBounds;
      glyphs.at(i).planeBounds = fontGlyph.planeBounds;

      glyphs.at(i).planeBounds.x += advanceX;
      glyphs.at(i).planeBounds.z += advanceX;
      glyphs.at(i).planeBounds.y -= advanceY;
      glyphs.at(i).planeBounds.w -= advanceY;

      advanceX += fontGlyph.advanceX;
    }

    frameData.addText(entity, font.deviceHandle, glyphs, world.worldTransform);
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
  for (auto [entity, environment] : entityDatabase.view<EnvironmentSkybox>()) {
    rhi::TextureHandle irradianceMap{0};
    rhi::TextureHandle specularMap{0};

    if (environment.type == EnvironmentSkyboxType::Color) {
      frameData.setSkyboxColor(environment.color);
    } else if (environment.type == EnvironmentSkyboxType::Texture &&
               mAssetRegistry.has(environment.texture)) {
      const auto &asset = mAssetRegistry.get(environment.texture).data;

      frameData.setSkyboxTexture(
          mAssetRegistry.get(asset.specularMap).data.deviceHandle);
      irradianceMap = mAssetRegistry.get(asset.irradianceMap).data.deviceHandle;
      specularMap = mAssetRegistry.get(asset.specularMap).data.deviceHandle;
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
                           rhi::PipelineHandle pipeline, u32 frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  u32 instanceStart = 0;
  for (auto &[handle, meshData] : frameData.getMeshGroups()) {
    const auto &mesh = mAssetRegistry.get(handle).data;
    u32 numInstances = static_cast<u32>(meshData.transforms.size());

    commandList.bindVertexBuffers(mesh.vertexBuffers, mesh.vertexBufferOffsets);
    commandList.bindIndexBuffer(mesh.indexBuffer, rhi::IndexType::Uint32);
    renderGeometries(commandList, pipeline, mesh, instanceStart, numInstances);
    instanceStart += numInstances;
  }
}

void SceneRenderer::renderSkinned(rhi::RenderCommandList &commandList,
                                  rhi::PipelineHandle pipeline,
                                  u32 frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  u32 instanceStart = 0;

  for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
    const auto &mesh = mAssetRegistry.get(handle).data;
    u32 numInstances = static_cast<u32>(meshData.transforms.size());

    commandList.bindVertexBuffers(mesh.vertexBuffers, mesh.vertexBufferOffsets);
    commandList.bindIndexBuffer(mesh.indexBuffer, rhi::IndexType::Uint32);

    renderGeometries(commandList, pipeline, mesh, instanceStart, numInstances);
    instanceStart += numInstances;
  }
}

void SceneRenderer::renderGeometries(rhi::RenderCommandList &commandList,
                                     rhi::PipelineHandle pipeline,
                                     const MeshAsset &mesh, u32 instanceStart,
                                     u32 numInstances) {
  i32 vertexOffset = 0;
  u32 indexOffset = 0;
  for (usize g = 0; g < mesh.geometries.size(); ++g) {
    auto &geometry = mesh.geometries.at(g);

    auto index = static_cast<u32>(g);

    commandList.pushConstants(pipeline, rhi::ShaderStage::Vertex, 0,
                              sizeof(u32), &index);

    u32 indexCount = static_cast<u32>(mesh.geometries.at(g).indices.size());
    i32 vertexCount = static_cast<i32>(mesh.geometries.at(g).positions.size());

    commandList.drawIndexed(indexCount, indexOffset, vertexOffset, numInstances,
                            instanceStart);
    vertexOffset += vertexCount;
    indexOffset += indexCount;
  }
}

void SceneRenderer::renderShadowsMesh(rhi::RenderCommandList &commandList,
                                      rhi::PipelineHandle pipeline,
                                      u32 frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  u32 instanceStart = 0;
  for (auto &[handle, meshData] : frameData.getMeshGroups()) {
    const auto &mesh = mAssetRegistry.get(handle).data;
    u32 numInstances = static_cast<u32>(meshData.transforms.size());

    commandList.bindVertexBuffers(
        MeshRenderUtils::getGeometryBuffers(mesh),
        MeshRenderUtils::getGeometryBufferOffsets(mesh));
    commandList.bindIndexBuffer(mesh.indexBuffer, rhi::IndexType::Uint32);
    renderShadowsGeometries(commandList, pipeline, mesh, instanceStart,
                            numInstances);
    instanceStart += numInstances;
  }
}

void SceneRenderer::renderShadowsSkinnedMesh(
    rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
    u32 frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);

  u32 instanceStart = 0;
  for (auto &[handle, meshData] : frameData.getSkinnedMeshGroups()) {
    const auto &mesh = mAssetRegistry.get(handle).data;
    u32 numInstances = static_cast<u32>(meshData.transforms.size());

    commandList.bindVertexBuffers(
        MeshRenderUtils::getSkinnedGeometryBuffers(mesh),
        MeshRenderUtils::getSkinnedGeometryBufferOffsets(mesh));
    commandList.bindIndexBuffer(mesh.indexBuffer, rhi::IndexType::Uint32);
    renderShadowsGeometries(commandList, pipeline, mesh, instanceStart,
                            numInstances);
    instanceStart += numInstances;
  }
}

void SceneRenderer::renderShadowsGeometries(rhi::RenderCommandList &commandList,
                                            rhi::PipelineHandle pipeline,
                                            const MeshAsset &mesh,
                                            u32 instanceStart,
                                            u32 numInstances) {
  i32 vertexOffset = 0;
  u32 indexOffset = 0;
  for (usize g = 0; g < mesh.geometries.size(); ++g) {
    auto &geometry = mesh.geometries.at(g);

    u32 indexCount = static_cast<u32>(mesh.geometries.at(g).indices.size());
    i32 vertexCount = static_cast<i32>(mesh.geometries.at(g).positions.size());

    commandList.drawIndexed(indexCount, indexOffset, vertexOffset, numInstances,
                            instanceStart);
    vertexOffset += vertexCount;
    indexOffset += indexCount;
  }
}

void SceneRenderer::renderText(rhi::RenderCommandList &commandList,
                               rhi::PipelineHandle pipeline, u32 frameIndex) {
  auto &frameData = mFrameData.at(frameIndex);
  static constexpr u32 QuadNumVertices = 6;
  for (usize i = 0; i < frameData.getTexts().size(); ++i) {
    const auto &text = frameData.getTexts().at(i);

    commandList.bindDescriptor(pipeline, 0,
                               mRenderStorage.getGlobalTexturesDescriptor());

    glm::uvec4 textConstants{
        rhi::castHandleToUint(text.fontTexture),
        rhi::castHandleToUint(mRenderStorage.getDefaultSampler()),
        text.glyphStart, 0};

    commandList.pushConstants(
        pipeline, rhi::ShaderStage::Vertex | rhi::ShaderStage::Fragment, 0,
        sizeof(glm::uvec4), glm::value_ptr(textConstants));

    commandList.draw(QuadNumVertices * static_cast<u32>(text.length), 0, 1,
                     static_cast<u32>(i));
  }
}

void SceneRenderer::generateBrdfLut() {
  auto *device = mRenderStorage.getDevice();

  static constexpr u32 GroupSize = 16;
  static constexpr u32 TextureSize = 512;

  rhi::DescriptorLayoutBindingDescription binding0{};
  binding0.type = rhi::DescriptorLayoutBindingType::Static;
  binding0.binding = 0;
  binding0.name = "uOutputTexture";
  binding0.shaderStage = rhi::ShaderStage::Compute;
  binding0.descriptorCount = 1;
  binding0.descriptorType = rhi::DescriptorType::StorageImage;

  auto layout = device->createDescriptorLayout({{binding0}});

  auto pipeline = mRenderStorage.addPipeline(rhi::ComputePipelineDescription(
      {mRenderStorage.getShader("__engine.pbr.brdfLut.compute"),
       "generate brdf lut"}));

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

} // namespace quoll
