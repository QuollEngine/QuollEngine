#pragma once

#include "renderer/render-graph/RenderGraph.h"

namespace liquid {

class VulkanRenderer;
class GLFWWindow;

class MainLoop {
public:
  MainLoop(VulkanRenderer *renderer, GLFWWindow *window);

  int run(RenderGraph &graph, const std::function<bool(double)> &update);

private:
  VulkanRenderer *renderer = nullptr;
  GLFWWindow *window = nullptr;
};
} // namespace liquid
