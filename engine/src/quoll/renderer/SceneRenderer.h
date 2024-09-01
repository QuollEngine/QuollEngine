#pragma once

#include "quoll/renderer/RenderGraph.h"
#include "quoll/renderer/RendererOptions.h"
#include "quoll/rhi/RenderCommandList.h"
#include "SceneRendererFrameData.h"

namespace quoll {

class AssetRegistry;
class RenderStorage;
class RendererAssetRegistry;
struct MeshDrawData;

struct SceneRenderPassData {
  RenderGraphResource<rhi::TextureHandle> sceneColor;

  RenderGraphResource<rhi::TextureHandle> sceneColorResolved;

  RenderGraphResource<rhi::TextureHandle> finalColor;

  RenderGraphResource<rhi::TextureHandle> depthBuffer;

  u32 sampleCount = 0;
};

class SceneRenderer {
  static constexpr glm::vec4 DefaultClearColor{0.0f, 0.0f, 0.0f, 1.0f};

public:
  SceneRenderer(AssetRegistry &assetRegistry, RenderStorage &renderStorage,
                RendererAssetRegistry &rendererAssetRegistry);

  void setClearColor(const glm::vec4 &clearColor);

  SceneRenderPassData attach(RenderGraph &graph,
                             const RendererOptions &options);

  void attachText(RenderGraph &graph, const SceneRenderPassData &passData);

  void updateFrameData(EntityDatabase &entityDatabase, Entity camera,
                       u32 frameIndex);

  inline const std::array<SceneRendererFrameData, 2> &getFrameData() {
    return mFrameData;
  }

private:
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              u32 frameIndex);

  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline, u32 frameIndex);

  void renderGeometries(rhi::RenderCommandList &commandList,
                        rhi::PipelineHandle pipeline,
                        const MeshDrawData *drawData, u32 instanceStart,
                        u32 numInstances);

  void renderShadowsMesh(rhi::RenderCommandList &commandList,
                         rhi::PipelineHandle pipeline, u32 frameIndex);

  void renderShadowsSkinnedMesh(rhi::RenderCommandList &commandList,
                                rhi::PipelineHandle pipeline, u32 frameIndex);

  void renderShadowsGeometries(rhi::RenderCommandList &commandList,
                               rhi::PipelineHandle pipeline,
                               const MeshDrawData *drawData, u32 instanceStart,
                               u32 numInstances);

  void renderText(rhi::RenderCommandList &commandList,
                  rhi::PipelineHandle pipeline, u32 frameIndex);

  void generateBrdfLut();

  inline u32 getFramebufferSamples() const { return mMaxSampleCounts; }

private:
  glm::vec4 mClearColor{DefaultClearColor};
  AssetRegistry &mAssetRegistry;
  RenderStorage &mRenderStorage;
  RendererAssetRegistry &mRendererAssetRegistry;

  std::array<SceneRendererFrameData, rhi::RenderDevice::NumFrames> mFrameData;

  rhi::SamplerHandle mBloomSampler;

  u32 mMaxSampleCounts = 1;
};

} // namespace quoll
