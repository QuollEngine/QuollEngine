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

int MainLoop::run(Scene *scene, const std::function<bool()> &updater) {

  bool running = true;

  const auto &renderData = renderer->prepareScene(scene);

  auto prevTime = std::chrono::high_resolution_clock::now();
  uint32_t frames = 0;

  constexpr uint32_t ONE_SECOND_IN_MS = 1000;

  while (running) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (window->shouldClose()) {
      break;
    }

    window->pollEvents();

    renderData->update();

    renderer->getImguiRenderer()->beginRendering();
    running = updater();

    debugLayer.render();
    renderer->getImguiRenderer()->endRendering();

    scene->update();

    renderer->draw(renderData);

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                              prevTime)
            .count() >= ONE_SECOND_IN_MS) {
      prevTime = currentTime;
      debugLayer.collectFPS(frames);
      frames = 0;
    } else {
      frames++;
    }
  }

  renderer->waitForIdle();

  return 0;
}

} // namespace liquid
