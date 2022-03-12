#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanShader.h"
#include "VulkanRenderData.h"
#include "VulkanAbstraction.h"
#include "VulkanGraphEvaluator.h"

#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/MaterialPBR.h"
#include "liquid/renderer/imgui/ImguiRenderer.h"

#include "liquid/entity/EntityContext.h"

#include "liquid/scene/Camera.h"
#include "liquid/scene/Scene.h"

#include "liquid/profiler/StatsManager.h"
#include "liquid/profiler/DebugManager.h"

#include "liquid/rhi/RenderCommandList.h"

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

  inline StatsManager &getStatsManager() {
    return abstraction.getStatsManager();
  }
  inline const SharedPtr<DebugManager> &getDebugManager() {
    return debugManager;
  }

  inline ShaderLibrary *getShaderLibrary() { return shaderLibrary; }

  RenderGraph
  createRenderGraph(const SharedPtr<VulkanRenderData> &renderData,
                    const String &imguiDep,
                    const std::function<void(TextureHandle)> &imUpdate);

  inline VulkanAbstraction &getRenderBackend() { return abstraction; }

private:
  void loadShaders();

private:
  VulkanAbstraction abstraction;

  ShaderLibrary *shaderLibrary = new ShaderLibrary;

  std::vector<SharedPtr<Material>> shadowMaterials;

  EntityContext &entityContext;

  SharedPtr<DebugManager> debugManager;
};

} // namespace liquid
