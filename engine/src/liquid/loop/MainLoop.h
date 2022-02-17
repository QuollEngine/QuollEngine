#pragma once

#include "liquid/renderer/render-graph/RenderGraph.h"

namespace liquid {

class VulkanRenderer;
class GLFWWindow;

class MainLoop {
public:
  MainLoop(VulkanRenderer *renderer, GLFWWindow *window);

  int run(RenderGraph &graph, const std::function<bool(float)> &update);

private:
  VulkanRenderer *renderer = nullptr;
  GLFWWindow *window = nullptr;
};
} // namespace liquid
