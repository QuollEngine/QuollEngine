#pragma once

#include "liquid/core/Base.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "liquid/renderer/ResourceAllocator.h"

#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanRenderData.h"
#include "VulkanResourceAllocator.h"
#include "VulkanRenderBackend.h"
#include "VulkanGraphEvaluator.h"

#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/MaterialPBR.h"
#include "liquid/renderer/imgui/ImguiRenderer.h"
#include "liquid/renderer/RenderCommandList.h"

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
                 bool enableValidations = false);
  ~VulkanRenderer();

  VulkanRenderer(const VulkanRenderer &rhs) = delete;
  VulkanRenderer(VulkanRenderer &&rhs) = delete;
  VulkanRenderer &operator=(const VulkanRenderer &rhs) = delete;
  VulkanRenderer &operator=(VulkanRenderer &&rhs) = delete;

  SharedPtr<Material>
  createMaterial(const SharedPtr<Shader> &vertexShader,
                 const SharedPtr<Shader> &fragmentShader,
                 const std::vector<SharedPtr<Texture>> &textures,
                 const std::vector<std::pair<String, Property>> &properties,
                 const CullMode &cullMode);
  SharedPtr<Material>
  createMaterialPBR(const MaterialPBR::Properties &properties,
                    const CullMode &cullMode);
  SharedPtr<VulkanShader> createShader(const String &shaderFile);

  inline ResourceAllocator *getResourceAllocator() {
    return renderBackend.getResourceAllocator();
  }

  SharedPtr<VulkanRenderData> prepareScene(Scene *scene);

  inline StatsManager &getStatsManager() {
    return renderBackend.getStatsManager();
  }
  inline const SharedPtr<DebugManager> &getDebugManager() {
    return debugManager;
  }

  inline const VulkanContext &getContext() {
    return renderBackend.getVulkanInstance();
  }

  inline ShaderLibrary *getShaderLibrary() { return shaderLibrary; }

  RenderGraph createRenderGraph(
      const SharedPtr<VulkanRenderData> &renderData, const String &imguiDep,
      const std::function<void(const SharedPtr<Texture> &)> &imUpdate);

  inline VulkanRenderBackend &getRenderBackend() { return renderBackend; }

private:
  void loadShaders();

private:
  VulkanRenderBackend renderBackend;

  ShaderLibrary *shaderLibrary = nullptr;

  std::vector<SharedPtr<Material>> shadowMaterials;

  EntityContext &entityContext;

  SharedPtr<DebugManager> debugManager;
};

} // namespace liquid
