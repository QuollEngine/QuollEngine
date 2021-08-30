#pragma once

#include "core/Base.h"
#include <imgui.h>

#include "window/glfw/GLFWWindow.h"
#include "renderer/vulkan/VulkanContext.h"
#include "renderer/vulkan/VulkanDescriptorManager.h"
#include "renderer/vulkan/VulkanSwapchain.h"
#include "renderer/vulkan/VulkanUploadContext.h"

namespace liquid {

class ImguiRenderer {
public:
  ImguiRenderer(GLFWWindow *window, const VulkanContext &vulkanContext,
                const VulkanSwapchain &swapchain, VkRenderPass renderPass,
                const VulkanUploadContext &uploadContext);
  ~ImguiRenderer();

  ImguiRenderer(const ImguiRenderer &rhs) = delete;
  ImguiRenderer(ImguiRenderer &&rhs) = delete;
  ImguiRenderer &operator=(const ImguiRenderer &rhs) = delete;
  ImguiRenderer &operator=(ImguiRenderer &&rhs) = delete;

  void beginRendering();
  void endRendering();

  void draw(VkCommandBuffer buffer);

private:
  void uploadFonts(const VulkanUploadContext &uploadContext);

private:
  VulkanDescriptorManager *descriptorManager;
  const VulkanContext &vulkanContext;
};

} // namespace liquid
