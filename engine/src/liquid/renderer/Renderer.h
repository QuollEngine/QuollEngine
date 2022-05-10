#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "SceneRenderer.h"
#include "RenderStorage.h"
#include "imgui/ImguiRenderer.h"

#include "liquid/entity/EntityContext.h"

#include "liquid/profiler/DebugManager.h"
#include "liquid/asset/AssetRegistry.h"

namespace liquid {

class Window;

/**
 * @brief Default graph resources
 */
struct DefaultGraphResources {
  /**
   * Main color texture
   */
  rhi::TextureHandle mainColor;

  /**
   * Depth buffer texture
   */
  rhi::TextureHandle depthBuffer;

  /**
   * Shadow map texture
   */
  rhi::TextureHandle shadowmap;

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
   * @brief Get debug manager
   *
   * @return Debug manager
   */
  inline DebugManager &getDebugManager() { return mDebugManager; }

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
   * @brief Create render graph
   *
   * @param useSwapchainForImgui Use swapchain for imgui
   * @return Render graph and resources
   */
  std::pair<rhi::RenderGraph, DefaultGraphResources>
  createRenderGraph(bool useSwapchainForImgui);

  /**
   * @brief Render
   *
   * @param graph Render graph
   * @param camera Camera entity
   * @param entityContext Entity context
   */
  void render(rhi::RenderGraph &graph, Entity camera,
              liquid::EntityContext &entityContext);

  /**
   * @brief Wait for device
   */
  inline void wait() { mDevice->waitForIdle(); }

  /**
   * @brief Get render storage
   *
   * @return Render storage
   */
  inline const RenderStorage &getRenderStorage() const {
    return mRenderStorage;
  }

private:
  /**
   * @brief Load engine shaders
   */
  void loadShaders();

  /**
   * @brief Update storage buffers
   *
   * @param entityContext Entity context
   */
  void updateStorageBuffers(EntityContext &entityContext);

private:
  rhi::ResourceRegistry mRegistry;
  rhi::RenderGraphEvaluator mGraphEvaluator;
  rhi::RenderDevice *mDevice;
  ImguiRenderer mImguiRenderer;
  ShaderLibrary mShaderLibrary;
  DebugManager mDebugManager;
  SceneRenderer mSceneRenderer;
  AssetRegistry &mAssetRegistry;

  RenderStorage mRenderStorage;
};

} // namespace liquid
