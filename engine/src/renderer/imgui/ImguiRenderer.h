#pragma once

#include "core/Base.h"
#include <imgui.h>

#include "window/glfw/GLFWWindow.h"
#include "renderer/vulkan/VulkanContext.h"
#include "renderer/vulkan/VulkanDescriptorManager.h"
#include "renderer/vulkan/VulkanSwapchain.h"
#include "renderer/vulkan/VulkanShader.h"
#include "renderer/vulkan/VulkanTextureBinder.h"
#include "renderer/ShaderLibrary.h"
#include "renderer/ResourceAllocator.h"
#include "renderer/HardwareBuffer.h"
#include "renderer/RenderCommandList.h"

namespace liquid {

class ImguiRenderer {
  struct FrameData {
    SharedPtr<HardwareBuffer> vertexBuffer = nullptr;
    SharedPtr<HardwareBuffer> indexBuffer = nullptr;
  };

public:
  ImguiRenderer(GLFWWindow *window, const VulkanContext &vulkanContext,
                VkRenderPass renderPass, ShaderLibrary *shaderLibrary,
                ResourceAllocator *resourceAllocator);
  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  void beginRendering();
  void endRendering();

  void draw(RenderCommandList &commandList);

private:
  void loadFonts();

  void createPipeline();

  void createDescriptorLayout();
  VkDescriptorSet createDescriptorFromTexture(Texture *texture);

  void setupRenderStates(ImDrawData *draw_data, RenderCommandList &commandList,
                         int fbWidth, int fbHeight);

private:
  VulkanDescriptorManager *descriptorManager;
  const VulkanContext &vulkanContext;

  ResourceAllocator *resourceAllocator = nullptr;

  VkRenderPass renderPass = VK_NULL_HANDLE;

  SharedPtr<Texture> fontTexture = nullptr;

  std::vector<FrameData> frameData;
  uint32_t currentFrame = 0;

  ShaderLibrary *shaderLibrary = nullptr;
  VkDescriptorSetLayout descriptorLayout = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  std::unordered_map<Texture *, VkDescriptorSet> descriptorMap;
};

} // namespace liquid
