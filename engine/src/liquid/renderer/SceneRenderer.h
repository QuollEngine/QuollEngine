#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/renderer/RenderGraph.h"
#include "liquid/asset/AssetRegistry.h"
#include "SceneRendererFrameData.h"
#include "ShaderLibrary.h"

namespace liquid {

/**
 * @brief Scene render pass data
 */
struct SceneRenderPassData {
  /**
   * Scene texture
   */
  rhi::TextureHandle sceneColor;

  /**
   * Scene color resolved
   */
  rhi::TextureHandle sceneColorResolved;

  /**
   * Final color texture
   */
  rhi::TextureHandle finalColor;

  /**
   * Scene depth buffer
   */
  rhi::TextureHandle depthBuffer;
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
   * @param shaderLibrary Shader library
   * @param assetRegistry Asset registry
   * @param renderStorage Render storage
   */
  SceneRenderer(ShaderLibrary &shaderLibrary, AssetRegistry &assetRegistry,
                RenderStorage &renderStorage);

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
   * @return Scene render pass data
   */
  SceneRenderPassData attach(RenderGraph &graph);

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
   * @param bindMaterialData Bind material data
   * @param frameIndex Frame index
   */
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              bool bindMaterialData, uint32_t frameIndex);

  /**
   * @brief Render skinned meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param bindMaterialData Bind material data
   * @param frameIndex Frame index
   */
  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline, bool bindMaterialData,
                     uint32_t frameIndex);

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
  ShaderLibrary &mShaderLibrary;
  AssetRegistry &mAssetRegistry;
  RenderStorage &mRenderStorage;
  rhi::RenderDevice *mDevice;
  std::array<SceneRendererFrameData, rhi::RenderDevice::NumFrames> mFrameData;

  uint32_t mMaxSampleCounts = 1;
};

} // namespace liquid
