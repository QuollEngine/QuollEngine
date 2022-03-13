#pragma once

#include "liquid/renderer/render-graph/RenderGraph.h"

namespace liquid {

class Renderer;
class Window;

class MainLoop {
public:
  MainLoop(Renderer &renderer, Window &window);

  int run(RenderGraph &graph, const std::function<bool(float)> &update);

private:
  Renderer &renderer;
  Window &window;
};
} // namespace liquid
