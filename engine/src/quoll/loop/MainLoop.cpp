#include "quoll/core/Base.h"
#include "quoll/window/Window.h"
#include "quoll/profiler/FPSCounter.h"

#include "MainLoop.h"

namespace quoll {

MainLoop::MainLoop(Window &window, FPSCounter &fpsCounter)
    : mWindow(window), mFpsCounter(fpsCounter) {}

void MainLoop::setUpdateFn(const std::function<void(f32)> &updateFn) {
  mUpdateFn = updateFn;
}

void MainLoop::setRenderFn(const std::function<void()> &renderFn) {
  mRenderFn = renderFn;
}

void MainLoop::stop() { mRunning = false; }

void MainLoop::run() {
  static constexpr u32 OneSecondInMs = 1000;
  static constexpr f64 MaxUpdateTime = 0.25;
  static constexpr f64 TimeDelta = 0.01;

  u32 frames = 0;
  f64 accumulator = 0.0;

  auto prevGameTime = std::chrono::high_resolution_clock::now();
  auto prevFrameTime = prevGameTime;
  while (mRunning) {
    QUOLL_PROFILE_FRAME("MainLoop");
    auto currentTime = std::chrono::high_resolution_clock::now();

    if (mWindow.shouldClose()) {
      break;
    }

    mWindow.pollEvents();

    f64 frameTime = std::clamp(
        std::chrono::duration<f64>(currentTime - prevGameTime).count(), 0.0,
        MaxUpdateTime);

    prevGameTime = currentTime;
    accumulator += frameTime;

    while (accumulator >= TimeDelta) {
      mUpdateFn(static_cast<f32>(TimeDelta));
      accumulator -= TimeDelta;
    }

    const auto &size = mWindow.getFramebufferSize();
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
