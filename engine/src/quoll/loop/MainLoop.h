#pragma once

namespace quoll {

class Window;
class FPSCounter;

/**
 * @brief Main loop
 *
 * Calls updater and renderer
 * in a loop and provides
 * performance metrics
 */
class MainLoop {
public:
  MainLoop(Window &window, FPSCounter &fpsCounter);

  void run();

  void setFixedUpdateFn(const std::function<void(f32)> &fixedUpdateFn);

  void setUpdateFn(const std::function<void(f32)> &updateFn);

  void setRenderFn(const std::function<void()> &renderFn);

  void setPrepareFn(const std::function<void()> &prepareFn);

  void stop();

private:
  bool mRunning = true;

  Window &mWindow;
  FPSCounter &mFpsCounter;
  std::function<void(f32)> mUpdateFn = [](f32) {};
  std::function<void(f32)> mFixedUpdateFn = [](f32) {};

  std::function<void()> mRenderFn = []() {};
  std::function<void()> mPrepareFn = []() {};
};
} // namespace quoll
