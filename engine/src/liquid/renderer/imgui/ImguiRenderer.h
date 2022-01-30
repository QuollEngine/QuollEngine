#pragma once

#include <imgui.h>

#include "liquid/window/glfw/GLFWWindow.h"
#include "liquid/renderer/vulkan/VulkanContext.h"
#include "liquid/renderer/ResourceAllocator.h"
#include "liquid/renderer/HardwareBuffer.h"
#include "liquid/renderer/RenderCommandList.h"
#include "liquid/renderer/Pipeline.h"

namespace liquid {

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

  static void beginRendering();
  static void endRendering();

  void draw(RenderCommandList &commandList,
            const SharedPtr<Pipeline> &pipeline);

private:
  void loadFonts();

  VkDescriptorSet
  createDescriptorFromTexture(Texture *texture,
                              const SharedPtr<Pipeline> &pipeline);

  void setupRenderStates(ImDrawData *draw_data, RenderCommandList &commandList,
                         int fbWidth, int fbHeight,
                         const SharedPtr<Pipeline> &pipeline);

private:
  const VulkanContext &vulkanContext;

  ResourceAllocator *resourceAllocator = nullptr;
  SharedPtr<Texture> fontTexture = nullptr;

  std::vector<FrameData> frameData;
  uint32_t currentFrame = 0;
};

} // namespace liquid
