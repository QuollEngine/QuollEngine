#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "renderer/ResourceAllocator.h"

#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanDescriptorManager.h"
#include "VulkanRenderData.h"
#include "VulkanDeferredResourceManager.h"
#include "VulkanResourceAllocator.h"
#include "VulkanRenderBackend.h"
#include "VulkanGraphEvaluator.h"

#include "renderer/ShaderLibrary.h"
#include "renderer/MaterialPBR.h"
#include "renderer/imgui/ImguiRenderer.h"
#include "renderer/RenderCommandList.h"

#include "entity/EntityContext.h"

#include "scene/Camera.h"
#include "scene/Scene.h"

#include "profiler/StatsManager.h"
#include "profiler/DebugManager.h"

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

  void draw(const SharedPtr<VulkanRenderData> &renderData);
  void waitForIdle();

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
  inline ImguiRenderer *getImguiRenderer() { return imguiRenderer; }

  SharedPtr<VulkanRenderData> prepareScene(Scene *scene);

  inline const StatsManager &getStatsManager() { return statsManager; }
  inline const SharedPtr<DebugManager> &getDebugManager() {
    return debugManager;
  }

  inline const VulkanContext &getContext() {
    return renderBackend.getVulkanInstance();
  }

  inline ShaderLibrary *getShaderLibrary() { return shaderLibrary; }

private:
  void createRenderGraph(const SharedPtr<VulkanRenderData> &renderData);

  void loadShaders();

  void createImgui();

private:
  VulkanRenderBackend renderBackend;

  VulkanDescriptorManager *descriptorManager = nullptr;
  VulkanDeferredResourceManager *deferredResourceManager = nullptr;
  ImguiRenderer *imguiRenderer = nullptr;
  ShaderLibrary *shaderLibrary = nullptr;

  std::vector<SharedPtr<Material>> shadowMaterials;

  RenderGraph graph;
  bool graphCreated = false;

  EntityContext &entityContext;

  SharedPtr<DebugManager> debugManager;
  StatsManager statsManager;
};

} // namespace liquid
