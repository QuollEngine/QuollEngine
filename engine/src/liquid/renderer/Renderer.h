#pragma once

#include "RenderData.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "SceneRenderer.h"
#include "imgui/ImguiRenderer.h"

#include "liquid/entity/EntityContext.h"
#include "liquid/rhi/RenderDevice.h"

#include "liquid/scene/Camera.h"
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
  Renderer(EntityContext &context, Window &window, rhi::RenderDevice *device);
  ~Renderer();

  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  SharedPtr<Material>
  createMaterial(const std::vector<rhi::TextureHandle> &textures,
                 const std::vector<std::pair<String, Property>> &properties,
                 const rhi::CullMode &cullMode);
  SharedPtr<Material>
  createMaterialPBR(const MaterialPBR::Properties &properties,
                    const rhi::CullMode &cullMode);

  SharedPtr<RenderData> prepareScene(Scene *scene);

  SharedPtr<MeshInstance<Mesh>> createMeshInstance(MeshAssetHandle mesh,
                                                   AssetRegistry &registry);

  SharedPtr<MeshInstance<SkinnedMesh>>
  createMeshInstance(SkinnedMeshAssetHandle handle, AssetRegistry &registry);

  inline DebugManager &getDebugManager() { return mDebugManager; }
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }
  inline rhi::ResourceRegistry &getRegistry() { return mRegistry; }
  inline rhi::RenderDevice *getRenderDevice() { return mDevice; }
  inline ImguiRenderer &getImguiRenderer() { return mImguiRenderer; }

  std::pair<rhi::RenderGraph, DefaultGraphResources>
  createRenderGraph(const SharedPtr<RenderData> &renderData,
                    bool useSwapchainForImgui);

  void render(rhi::RenderGraph &graph);

  inline void wait() { mDevice->waitForIdle(); }

private:
  void loadShaders();

private:
  EntityContext &mEntityContext;
  rhi::ResourceRegistry mRegistry;
  rhi::RenderGraphEvaluator mGraphEvaluator;
  rhi::RenderDevice *mDevice;
  ImguiRenderer mImguiRenderer;
  ShaderLibrary mShaderLibrary;
  DebugManager mDebugManager;
  SceneRenderer mSceneRenderer;

  std::vector<SharedPtr<Material>> mShadowMaterials;
};

} // namespace liquid
