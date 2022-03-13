#include "liquid/core/Base.h"

#include "MainLoop.h"
#include "liquid/renderer/vulkan/VulkanRenderer.h"
#include "liquid/window/glfw/GLFWWindow.h"

namespace liquid {

MainLoop::MainLoop(VulkanRenderer *renderer_, GLFWWindow *window_)
    : renderer(renderer_), window(window_) {}

int MainLoop::run(RenderGraph &graph,
                  const std::function<bool(float)> &updater) {

  bool running = true;

  constexpr uint32_t ONE_SECOND_IN_MS = 1000;
  constexpr double MAX_UPDATE_TIME = 0.25;
  constexpr double dt = 0.01;

  uint32_t frames = 0;
  double accumulator = 0.0;

  auto prevGameTime = std::chrono::high_resolution_clock::now();
  auto prevFrameTime = prevGameTime;
  while (running) {
    LIQUID_PROFILE_FRAME("MainLoop");
    auto currentTime = std::chrono::high_resolution_clock::now();

    if (window->shouldClose()) {
      break;
    }

    window->pollEvents();

    double frameTime = std::clamp(
        std::chrono::duration<double>(currentTime - prevGameTime).count(), 0.0,
        MAX_UPDATE_TIME);

    prevGameTime = currentTime;
    accumulator += frameTime;

    while (accumulator >= dt) {
      running = updater(static_cast<float>(dt));
      accumulator -= dt;
    }

    renderer->render(graph);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                              prevFrameTime)
            .count() >= ONE_SECOND_IN_MS) {
      prevFrameTime = currentTime;
      renderer->getStatsManager().collectFPS(frames);
      frames = 0;
    } else {
      frames++;
    }
  }

  renderer->wait();

  return 0;
}

} // namespace liquid
