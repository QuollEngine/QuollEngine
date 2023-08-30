#include "quoll/core/Base.h"
#include "quoll/window/Window.h"
#include "quoll/profiler/FPSCounter.h"

#include "MainLoop.h"

namespace quoll {

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

  static constexpr uint32_t OneSecondInMs = 1000;
  static constexpr double MaxUpdateTime = 0.25;
  static constexpr double TimeDelta = 0.01;

  uint32_t frames = 0;
  double accumulator = 0.0;

  auto prevGameTime = std::chrono::high_resolution_clock::now();
  auto prevFrameTime = prevGameTime;
  while (running) {
    QUOLL_PROFILE_FRAME("MainLoop");
    auto currentTime = std::chrono::high_resolution_clock::now();

    if (mWindow.shouldClose()) {
      break;
    }

    mWindow.pollEvents();

    double frameTime = std::clamp(
        std::chrono::duration<double>(currentTime - prevGameTime).count(), 0.0,
        MaxUpdateTime);

    prevGameTime = currentTime;
    accumulator += frameTime;

    while (accumulator >= TimeDelta) {
      running = mUpdateFn(static_cast<float>(TimeDelta));
      accumulator -= TimeDelta;
    }

    const auto &size = mWindow.getWindowSize();
    if (size.x > 0 && size.y > 0) {
      mRenderFn();
    }

    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                              prevFrameTime)
            .count() >= OneSecondInMs) {
      prevFrameTime = currentTime;
      mFpsCounter.collectFPS(frames);
      frames = 0;
    } else {
      frames++;
    }
  }
}

} // namespace quoll
