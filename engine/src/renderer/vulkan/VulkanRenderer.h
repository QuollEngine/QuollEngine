#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "renderer/ResourceAllocator.h"

#include "VulkanSwapchain.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanShader.h"
#include "VulkanDescriptorManager.h"
#include "VulkanRenderData.h"
#include "VulkanResourceManager.h"
#include "VulkanShadowPass.h"
#include "VulkanResourceAllocator.h"

#include "VulkanRenderBackend.h"

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

  void drawRenderables(RenderCommandList &commandList,
                       const SharedPtr<Camera> &camera,
                       bool useForShadowMapping);
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
  inline VulkanPipelineBuilder *getPipelineBuilder() { return pipelineBuilder; }
  inline ImguiRenderer *getImguiRenderer() { return imguiRenderer; }

  SharedPtr<VulkanRenderData> prepareScene(Scene *scene);

  void setClearColor(glm::vec4 clearColor);

  inline const StatsManager &getStatsManager() { return statsManager; }
  inline const SharedPtr<DebugManager> &getDebugManager() {
    return debugManager;
  }

  inline const VulkanContext &getContext() {
    return renderBackend.getVulkanInstance();
  }

  inline ShaderLibrary *getShaderLibrary() { return shaderLibrary; }

private:
  void createRenderPass();
  void createPipelineBuilder();

  void loadShaders();

  void createShadowPass();
  void destroyShadowPass();

  void createImgui();

  void setViewportAndScissor(RenderCommandList &commandList, glm::vec2 extent);

private:
  glm::vec4 clearColor{0.0, 0.0, 0.0, 1.0};

  bool framebufferResized = false;

  VulkanRenderBackend renderBackend;

  VulkanDescriptorManager *descriptorManager = nullptr;
  VulkanPipelineBuilder *pipelineBuilder = nullptr;
  VulkanResourceManager *mainResourceManager = nullptr;

  SharedPtr<VulkanShadowPass> shadowPass;
  std::vector<SharedPtr<Material>> shadowMaterials;

  ImguiRenderer *imguiRenderer = nullptr;

  ShaderLibrary *shaderLibrary;

  EntityContext &entityContext;

  SharedPtr<DebugManager> debugManager;
  StatsManager statsManager;
};

} // namespace liquid
