#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "vulkan/VulkanShader.h"

#include "RenderData.h"
#include "ShaderLibrary.h"
#include "MaterialPBR.h"
#include "imgui/ImguiRenderer.h"
#include "render-graph/RenderGraph.h"
#include "render-graph/RenderGraphEvaluator.h"

#include "liquid/rhi/vulkan/VulkanRenderDevice.h"

#include "liquid/entity/EntityContext.h"

#include "liquid/scene/Camera.h"
#include "liquid/scene/Scene.h"

#include "liquid/profiler/StatsManager.h"
#include "liquid/profiler/DebugManager.h"

namespace liquid {

class Window;

class Renderer {
public:
  Renderer(EntityContext &context, Window &window,
           experimental::VulkanRenderDevice *device);
  ~Renderer();

  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  SharedPtr<Material>
  createMaterial(const SharedPtr<Shader> &vertexShader,
                 const SharedPtr<Shader> &fragmentShader,
                 const std::vector<TextureHandle> &textures,
                 const std::vector<std::pair<String, Property>> &properties,
                 const CullMode &cullMode);
  SharedPtr<Material>
  createMaterialPBR(const MaterialPBR::Properties &properties,
                    const CullMode &cullMode);
  SharedPtr<VulkanShader> createShader(const String &shaderFile);

  SharedPtr<RenderData> prepareScene(Scene *scene);

  inline StatsManager &getStatsManager() { return mStatsManager; }
  inline DebugManager &getDebugManager() { return mDebugManager; }
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }
  inline experimental::ResourceRegistry &getRegistry() { return mRegistry; }
  inline experimental::VulkanRenderDevice *getRenderDevice() { return mDevice; }
  inline ImguiRenderer &getImguiRenderer() { return mImguiRenderer; }

  RenderGraph
  createRenderGraph(const SharedPtr<RenderData> &renderData,
                    const String &imguiDep,
                    const std::function<void(TextureHandle)> &imUpdate);

  void render(RenderGraph &graph);

  inline void wait() { mDevice->wait(); }

private:
  void loadShaders();

private:
  EntityContext &mEntityContext;
  experimental::ResourceRegistry mRegistry;
  RenderGraphEvaluator mGraphEvaluator;
  experimental::VulkanRenderDevice *mDevice;
  ImguiRenderer mImguiRenderer;
  StatsManager mStatsManager;
  ShaderLibrary mShaderLibrary;
  DebugManager mDebugManager;

  std::vector<SharedPtr<Material>> mShadowMaterials;
};

} // namespace liquid
