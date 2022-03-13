#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanShader.h"
#include "VulkanRenderData.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/MaterialPBR.h"
#include "liquid/renderer/imgui/ImguiRenderer.h"
#include "liquid/renderer/render-graph/RenderGraph.h"
#include "liquid/renderer/render-graph/RenderGraphEvaluator.h"

#include "liquid/rhi/vulkan/VulkanRenderDevice.h"

#include "liquid/entity/EntityContext.h"

#include "liquid/scene/Camera.h"
#include "liquid/scene/Scene.h"

#include "liquid/profiler/StatsManager.h"
#include "liquid/profiler/DebugManager.h"

namespace liquid {

class GLFWWindow;

class VulkanRenderer {
public:
  VulkanRenderer(EntityContext &context, GLFWWindow *window,
                 experimental::VulkanRenderDevice *device);
  ~VulkanRenderer();

  VulkanRenderer(const VulkanRenderer &rhs) = delete;
  VulkanRenderer(VulkanRenderer &&rhs) = delete;
  VulkanRenderer &operator=(const VulkanRenderer &rhs) = delete;
  VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

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

  SharedPtr<VulkanRenderData> prepareScene(Scene *scene);

  inline StatsManager &getStatsManager() { return mStatsManager; }
  inline DebugManager &getDebugManager() { return mDebugManager; }
  inline ShaderLibrary &getShaderLibrary() { return mShaderLibrary; }
  inline experimental::ResourceRegistry &getRegistry() { return mRegistry; }
  inline experimental::VulkanRenderDevice *getRenderDevice() { return mDevice; }
  inline ImguiRenderer &getImguiRenderer() { return mImguiRenderer; }

  RenderGraph
  createRenderGraph(const SharedPtr<VulkanRenderData> &renderData,
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
