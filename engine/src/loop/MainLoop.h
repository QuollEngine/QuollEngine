#pragma once

#include "core/Base.h"

#include "renderer/render-graph/RenderGraph.h"
#include "profiler/ImguiDebugLayer.h"

namespace liquid {

class VulkanRenderer;
class GLFWWindow;
class VulkanMaterial;
class Scene;

class MainLoop {
public:
  MainLoop(VulkanRenderer *renderer, GLFWWindow *window);

  int run(RenderGraph &graph, const std::function<bool(double)> &update,
          const std::function<void()> &renderUI);

private:
  VulkanRenderer *renderer = nullptr;
  GLFWWindow *window = nullptr;
  ImguiDebugLayer debugLayer;
};
} // namespace liquid
