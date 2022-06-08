#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "SceneRenderer.h"
#include "RenderStorage.h"
#include "liquid/imgui/ImguiRenderer.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/asset/AssetRegistry.h"

namespace liquid {

class Window;

/**
 * @brief Default graph resources
 */
struct DefaultGraphResources {
  /**
   * Scene color texture
   */
  rhi::TextureHandle sceneColor;

  /**
   * Depth buffer texture
   */
  rhi::TextureHandle depthBuffer;

  /**
   * Imgui texture
   */
  rhi::TextureHandle imguiColor;

  /**
   * Default color
   */
  glm::vec4 defaultColor;
};

/**
 * @brief Renderer
 *
 * Executes the graph in the render device
 */
class Renderer {
public:
  /**
   * @brief Create renderer
   *
   * @param assetRegistry Asset registry
   * @param window Window
   * @param device Render device
   */
  Renderer(AssetRegistry &assetRegistry, Window &window,
           rhi::RenderDevice *device);

  ~Renderer() = default;
  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  /**
   * @brief Get shader library
   *
   * @return Shader library
   */
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }

  /**
   * @brief Get resource registry
   *
   * @return Resource registry
   */
  inline rhi::ResourceRegistry &getRegistry() { return mRegistry; }

  /**
   * @brief Get render device
   *
   * @return Render device
   */
  inline rhi::RenderDevice *getRenderDevice() { return mDevice; }

  /**
   * @brief Get imgui renderer
   *
   * @return Imgui renderer
   */
  inline ImguiRenderer &getImguiRenderer() { return mImguiRenderer; }

  /**
   * @brief Get scene renderer
   *
   * @return Scene renderer
   */
  inline SceneRenderer &getSceneRenderer() { return mSceneRenderer; }

  /**
   * @brief Render
   *
   * @param graph Render graph
   * @param commandList Render command list
   */
  void render(rhi::RenderGraph &graph, rhi::RenderCommandList &commandList);

  /**
   * @brief Wait for device
   */
  inline void wait() { mDevice->waitForIdle(); }

private:
  rhi::ResourceRegistry mRegistry;
  rhi::RenderGraphEvaluator mGraphEvaluator;
  rhi::RenderDevice *mDevice;
  ShaderLibrary mShaderLibrary;
  AssetRegistry &mAssetRegistry;
  ImguiRenderer mImguiRenderer;
  SceneRenderer mSceneRenderer;
};

} // namespace liquid
