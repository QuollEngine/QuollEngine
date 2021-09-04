#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "renderer/ResourceAllocator.h"

#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanShader.h"
#include "VulkanDescriptorManager.h"
#include "VulkanRenderData.h"
#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanResourceManager.h"
#include "VulkanShadowPass.h"
#include "VulkanResourceAllocator.h"

#include "renderer/ShaderLibrary.h"
#include "renderer/MaterialPBR.h"
#include "renderer/imgui/ImguiRenderer.h"

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

  void drawRenderables(VkCommandBuffer commandBuffer, Camera *camera,
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

  inline ResourceAllocator *getResourceAllocator() { return resourceAllocator; }
  inline VulkanPipelineBuilder *getPipelineBuilder() { return pipelineBuilder; }
  inline ImguiRenderer *getImguiRenderer() { return imguiRenderer; }

  SharedPtr<VulkanRenderData> prepareScene(Scene *scene);

  void setClearColor(glm::vec4 clearColor);

  inline const VulkanContext &getContext() const { return context; }

  inline const SharedPtr<StatsManager> &getStatsManager() {
    return statsManager;
  }
  inline const SharedPtr<DebugManager> &getDebugManager() {
    return debugManager;
  }

  inline ShaderLibrary *getShaderLibrary() { return shaderLibrary; }

private:
  void createAllocator();
  void createRenderPass();
  void createPipelineBuilder();

  void loadShaders();

  void createSwapchain();
  void recreateSwapchain();

  void createMainPass();
  void destroyMainPass();
  void createMainFramebuffers();
  void destroyMainFramebuffers();

  void createShadowPass();
  void destroyShadowPass();

  void createImgui();

  void setViewportAndScissor(VkCommandBuffer commandBuffer, VkExtent2D extent);

private:
  glm::vec4 clearColor{0.0, 0.0, 0.0, 1.0};

  uint32_t resizeHandler = 0;

  VulkanUploadContext uploadContext;
  VulkanRenderContext renderContext;
  VulkanDescriptorManager *descriptorManager = nullptr;
  VulkanPipelineBuilder *pipelineBuilder = nullptr;

  VkRenderPass mainRenderPass = nullptr;
  std::vector<VkFramebuffer> mainFramebuffers;
  VulkanResourceManager *mainResourceManager = nullptr;

  SharedPtr<VulkanShadowPass> shadowPass;
  std::vector<SharedPtr<Material>> shadowMaterials;

  ImguiRenderer *imguiRenderer = nullptr;

  ShaderLibrary *shaderLibrary;

  bool framebufferResized = false;

  VulkanSwapchain swapchain;

  ResourceAllocator *resourceAllocator = nullptr;
  VmaAllocator allocator = nullptr;

  VulkanContext context;

  GLFWWindow *window;

  EntityContext &entityContext;

  SharedPtr<DebugManager> debugManager;
  SharedPtr<StatsManager> statsManager;
};

} // namespace liquid
