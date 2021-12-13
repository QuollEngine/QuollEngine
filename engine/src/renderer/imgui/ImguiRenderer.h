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
#include "renderer/Pipeline.h"

namespace liquid {

class VulkanPipeline;

class ImguiRenderer {
  struct FrameData {
    SharedPtr<HardwareBuffer> vertexBuffer = nullptr;
    SharedPtr<HardwareBuffer> indexBuffer = nullptr;
  };

public:
  ImguiRenderer(GLFWWindow *window, const VulkanContext &vulkanContext,
                ResourceAllocator *resourceAllocator);
  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  void beginRendering();
  void endRendering();

  void draw(RenderCommandList &commandList,
            const SharedPtr<Pipeline> &pipeline);

private:
  void loadFonts();

  VkDescriptorSet
  createDescriptorFromTexture(Texture *texture,
                              const SharedPtr<Pipeline> &pipeline);

  void setupRenderStates(ImDrawData *draw_data, RenderCommandList &commandList,
                         int fbWidth, int fbHeight,
                         const SharedPtr<VulkanPipeline> &pipeline);

private:
  VulkanDescriptorManager *descriptorManager;
  const VulkanContext &vulkanContext;

  ResourceAllocator *resourceAllocator = nullptr;

  VkRenderPass renderPass = VK_NULL_HANDLE;

  SharedPtr<Texture> fontTexture = nullptr;

  std::vector<FrameData> frameData;
  uint32_t currentFrame = 0;

  std::unordered_map<Texture *, VkDescriptorSet> descriptorMap;
};

} // namespace liquid
