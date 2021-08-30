#pragma once

#include "core/Base.h"

#include "profiler/ImguiDebugLayer.h"

namespace liquid {

class VulkanRenderer;
class GLFWWindow;
class VulkanMaterial;
class Scene;

class MainLoop {
public:
  MainLoop(VulkanRenderer *renderer, GLFWWindow *window);

  int run(Scene *scene, const std::function<bool()> &updater);

private:
  VulkanRenderer *renderer = nullptr;
  GLFWWindow *window = nullptr;
  ImguiDebugLayer debugLayer;
};
} // namespace liquid
