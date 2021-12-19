#include "core/Base.h"
#include "MainLoop.h"
#include "renderer/vulkan/VulkanRenderer.h"
#include "window/glfw/GLFWWindow.h"
#include "scene/Scene.h"

namespace liquid {

MainLoop::MainLoop(VulkanRenderer *renderer_, GLFWWindow *window_)
    : renderer(renderer_), window(window_),
      debugLayer(renderer->getContext().getPhysicalDevice().getDeviceInfo(),
                 renderer->getStatsManager(), renderer->getDebugManager()) {}

int MainLoop::run(RenderGraph &graph,
                  const std::function<bool(double)> &updater,
                  const std::function<void()> &renderUI) {

  bool running = true;

  constexpr uint32_t ONE_SECOND_IN_MS = 1000;
  constexpr double MAX_UPDATE_TIME = 0.25;
  constexpr double dt = 0.01;

  uint32_t frames = 0;
  double accumulator = 0.0;

  auto prevGameTime = std::chrono::high_resolution_clock::now();
  auto prevFrameTime = prevGameTime;
  while (running) {
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
      running = updater(dt);
      accumulator -= dt;
    }

    ImguiRenderer::beginRendering();
    renderUI();
    debugLayer.render();
    ImguiRenderer::endRendering();

    renderer->getRenderBackend().execute(graph);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                              prevFrameTime)
            .count() >= ONE_SECOND_IN_MS) {
      prevFrameTime = currentTime;
      debugLayer.collectFPS(frames);
      frames = 0;
    } else {
      frames++;
    }
  }

  renderer->getRenderBackend().waitForIdle();

  return 0;
}

} // namespace liquid
