#pragma once

#include "RenderData.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "imgui/ImguiRenderer.h"
#include "render-graph/RenderGraph.h"
#include "render-graph/RenderGraphEvaluator.h"

#include "liquid/rhi/base/RenderDevice.h"

#include "liquid/entity/EntityContext.h"

#include "liquid/scene/Camera.h"
#include "liquid/scene/Scene.h"

#include "liquid/profiler/StatsManager.h"
#include "liquid/profiler/DebugManager.h"

namespace liquid {

class Window;

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
                 const CullMode &cullMode);
  SharedPtr<Material>
  createMaterialPBR(const MaterialPBR::Properties &properties,
                    const CullMode &cullMode);

  SharedPtr<RenderData> prepareScene(Scene *scene);

  inline StatsManager &getStatsManager() { return mStatsManager; }
  inline DebugManager &getDebugManager() { return mDebugManager; }
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }
  inline rhi::ResourceRegistry &getRegistry() { return mRegistry; }
  inline rhi::RenderDevice *getRenderDevice() { return mDevice; }
  inline ImguiRenderer &getImguiRenderer() { return mImguiRenderer; }

  RenderGraph createRenderGraph(const SharedPtr<RenderData> &renderData,
                                const String &imguiDep);

  void render(RenderGraph &graph);

  inline void wait() { mDevice->waitForIdle(); }

private:
  void loadShaders();

private:
  EntityContext &mEntityContext;
  rhi::ResourceRegistry mRegistry;
  RenderGraphEvaluator mGraphEvaluator;
  rhi::RenderDevice *mDevice;
  ImguiRenderer mImguiRenderer;
  StatsManager mStatsManager;
  ShaderLibrary mShaderLibrary;
  DebugManager mDebugManager;

  std::vector<SharedPtr<Material>> mShadowMaterials;
};

} // namespace liquid
