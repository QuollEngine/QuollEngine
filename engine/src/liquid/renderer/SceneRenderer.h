#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/renderer/RenderGraph.h"
#include "liquid/asset/AssetRegistry.h"
#include "liquid/renderer/RendererOptions.h"
#include "SceneRendererFrameData.h"

namespace quoll {

/**
 * @brief Scene render pass data
 */
struct SceneRenderPassData {
  /**
   * Scene texture
   */
  RenderGraphResource<rhi::TextureHandle> sceneColor;

  /**
   * Scene color resolved
   */
  RenderGraphResource<rhi::TextureHandle> sceneColorResolved;

  /**
   * Final color texture
   */
  RenderGraphResource<rhi::TextureHandle> finalColor;

  /**
   * Scene depth buffer
   */
  RenderGraphResource<rhi::TextureHandle> depthBuffer;

  /**
   * Sample count
   */
  uint32_t sampleCount = 0;
};

/**
 * @brief Scene renderer
 */
class SceneRenderer {
  static constexpr glm::vec4 DefaultClearColor{0.0f, 0.0f, 0.0f, 1.0f};

public:
  /**
   * @brief Create scene renderer
   *
   * @param assetRegistry Asset registry
   * @param renderStorage Render storage
   */
  SceneRenderer(AssetRegistry &assetRegistry, RenderStorage &renderStorage);

  /**
   * @brief Set clear color
   *
   * @param clearColor Clear color
   */
  void setClearColor(const glm::vec4 &clearColor);

  /**
   * @brief Attach passes to render graph
   *
   * @param graph Render graph
   * @param options Renderer options
   * @return Scene render pass data
   */
  SceneRenderPassData attach(RenderGraph &graph,
                             const RendererOptions &options);

  /**
   * @brief Attach text pass to render graph
   *
   * @param graph Render graph
   * @param passData Scene render pass data
   */
  void attachText(RenderGraph &graph, const SceneRenderPassData &passData);

  /**
   * @brief Update frame data
   *
   * @param entityDatabase Entity database
   * @param camera Camera entity
   * @param frameIndex Frame index
   */
  void updateFrameData(EntityDatabase &entityDatabase, Entity camera,
                       uint32_t frameIndex);

  /**
   * @brief Get frame data
   *
   * @return Scene renderer frame data
   */
  inline const std::array<SceneRendererFrameData, 2> &getFrameData() {
    return mFrameData;
  }

private:
  /**
   * @brief Render meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param frameIndex Frame index
   */
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              uint32_t frameIndex);

  /**
   * @brief Render skinned meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param frameIndex Frame index
   */
  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline, uint32_t frameIndex);

  /**
   * @brief Render geometries
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param mesh Mesh asset
   * @param instanceStart Instance start
   * @param numInstances Instance count
   */
  void renderGeometries(rhi::RenderCommandList &commandList,
                        rhi::PipelineHandle pipeline, const MeshAsset &mesh,
                        uint32_t instanceStart, uint32_t numInstances);

  /**
   * @brief Render meshes for shadows
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param frameIndex Frame index
   */
  void renderShadowsMesh(rhi::RenderCommandList &commandList,
                         rhi::PipelineHandle pipeline, uint32_t frameIndex);

  /**
   * @brief Render skinned meshes for shadows
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param frameIndex Frame index
   */
  void renderShadowsSkinnedMesh(rhi::RenderCommandList &commandList,
                                rhi::PipelineHandle pipeline,
                                uint32_t frameIndex);

  /**
   * @brief Render geometries for shadows
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param mesh Mesh asset
   * @param instanceStart Instance start
   * @param numInstances Instance count
   */
  void renderShadowsGeometries(rhi::RenderCommandList &commandList,
                               rhi::PipelineHandle pipeline,
                               const MeshAsset &mesh, uint32_t instanceStart,
                               uint32_t numInstances);

  /**
   * @brief Render texts
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param frameIndex Frame index
   */
  void renderText(rhi::RenderCommandList &commandList,
                  rhi::PipelineHandle pipeline, uint32_t frameIndex);

  /**
   * @brief Generate BRDF lookup table
   */
  void generateBrdfLut();

  /**
   * @brief Get framebuffer samples
   *
   * @return Framebuffer samples
   */
  inline uint32_t getFramebufferSamples() const { return mMaxSampleCounts; }

private:
  glm::vec4 mClearColor{DefaultClearColor};
  AssetRegistry &mAssetRegistry;
  RenderStorage &mRenderStorage;
  std::array<SceneRendererFrameData, rhi::RenderDevice::NumFrames> mFrameData;

  rhi::SamplerHandle mBloomSampler;

  uint32_t mMaxSampleCounts = 1;
};

} // namespace quoll
