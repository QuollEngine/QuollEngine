#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "SceneRenderer.h"
#include "RenderStorage.h"
#include "imgui/ImguiRenderer.h"

#include "liquid/entity/EntityContext.h"

#include "liquid/scene/Scene.h"

#include "liquid/profiler/DebugManager.h"
#include "liquid/asset/AssetRegistry.h"

namespace liquid {

class Window;

struct DefaultGraphResources {
  rhi::TextureHandle mainColor;
  rhi::TextureHandle depthBuffer;
  rhi::TextureHandle shadowmap;
  glm::vec4 defaultColor;
};

class Renderer {
public:
  Renderer(EntityContext &context, AssetRegistry &assetRegistry, Window &window,
           rhi::RenderDevice *device);

  ~Renderer() = default;
  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  inline DebugManager &getDebugManager() { return mDebugManager; }
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }
  inline rhi::ResourceRegistry &getRegistry() { return mRegistry; }
  inline rhi::RenderDevice *getRenderDevice() { return mDevice; }
  inline ImguiRenderer &getImguiRenderer() { return mImguiRenderer; }

  std::pair<rhi::RenderGraph, DefaultGraphResources>
  createRenderGraph(bool useSwapchainForImgui);

  void render(rhi::RenderGraph &graph, Entity entity);

  inline void wait() { mDevice->waitForIdle(); }

  inline const RenderStorage &getRenderStorage() const {
    return mRenderStorage;
  }

private:
  void loadShaders();

  void updateStorageBuffers();

private:
  EntityContext &mEntityContext;
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
