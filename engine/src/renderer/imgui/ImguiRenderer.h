#pragma once

#include "core/Base.h"
#include <imgui.h>

#include "window/glfw/GLFWWindow.h"
#include "renderer/vulkan/VulkanContext.h"
#include "renderer/vulkan/VulkanDescriptorManager.h"
#include "renderer/vulkan/VulkanSwapchain.h"
#include "renderer/vulkan/VulkanUploadContext.h"
#include "renderer/vulkan/VulkanShader.h"
#include "renderer/vulkan/VulkanTextureBinder.h"
#include "renderer/ShaderLibrary.h"
#include "renderer/ResourceAllocator.h"
#include "renderer/HardwareBuffer.h"

namespace liquid {

class ImguiRenderer {
  struct FrameData {
    SharedPtr<HardwareBuffer> vertexBuffer = nullptr;
    SharedPtr<HardwareBuffer> indexBuffer = nullptr;
  };

public:
  ImguiRenderer(GLFWWindow *window, const VulkanContext &vulkanContext,
                const VulkanSwapchain &swapchain, VkRenderPass renderPass,
                const VulkanUploadContext &uploadContext,
                ShaderLibrary *shaderLibrary,
                ResourceAllocator *resourceAllocator);
  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  void beginRendering();
  void endRendering();

  void draw(VkCommandBuffer buffer);

  void createEverything();

private:
  void loadFonts();

  void setupRenderStates(ImDrawData *draw_data, VkCommandBuffer commandBuffer,
                         int fbWidth, int fbHeight);

private:
  VulkanDescriptorManager *descriptorManager;
  const VulkanContext &vulkanContext;

  ResourceAllocator *resourceAllocator = nullptr;

  VkRenderPass renderPass = nullptr;

  SharedPtr<Texture> fontTexture = nullptr;

  std::vector<FrameData> frameData;
  uint32_t currentFrame = 0;

  ShaderLibrary *shaderLibrary = nullptr;
  VkDescriptorSetLayout descriptorLayout = nullptr;
  VkDescriptorSet descriptorSet = nullptr;
  VkPipelineLayout pipelineLayout = nullptr;
  VkPipeline pipeline = nullptr;
};

} // namespace liquid
