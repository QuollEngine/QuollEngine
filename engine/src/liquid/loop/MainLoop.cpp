#include "liquid/core/Base.h"
#include "liquid/window/Window.h"
#include "liquid/profiler/FPSCounter.h"

#include "MainLoop.h"

namespace liquid {

MainLoop::MainLoop(Window &window, FPSCounter &fpsCounter)
    : mWindow(window), mFpsCounter(fpsCounter) {}

void MainLoop::setUpdateFn(const std::function<bool(float)> &updateFn) {
  mUpdateFn = updateFn;
}

void MainLoop::setRenderFn(const std::function<void()> &renderFn) {
  mRenderFn = renderFn;
}

void MainLoop::run() {
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

    if (mWindow.shouldClose()) {
      break;
    }

    mWindow.pollEvents();

    double frameTime = std::clamp(
        std::chrono::duration<double>(currentTime - prevGameTime).count(), 0.0,
        MAX_UPDATE_TIME);

    prevGameTime = currentTime;
    accumulator += frameTime;

    while (accumulator >= dt) {
      running = mUpdateFn(static_cast<float>(dt));
      accumulator -= dt;
    }

    mRenderFn();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                              prevFrameTime)
            .count() >= ONE_SECOND_IN_MS) {
      prevFrameTime = currentTime;
      mFpsCounter.collectFPS(frames);
      frames = 0;
    } else {
      frames++;
    }
  }
}

} // namespace liquid
