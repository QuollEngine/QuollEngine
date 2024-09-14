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

  void setFixedUpdateFn(std::function<void(f32)> &&fixedUpdateFn);

  void setUpdateFn(std::function<void(f32)> &&updateFn);

  void setRenderFn(std::function<void()> &&renderFn);

  void setPrepareFn(std::function<void()> &&prepareFn);

  /**
   * @brief Set stats function
   *
   * Stats function is performed after render and once every second.
   *
   * @param statsFn Callback that accepts number of frames
   */
  void setStatsFn(std::function<void(u32)> &&statsFn);

  void stop();

private:
  bool mRunning = true;

  Window &mWindow;
  FPSCounter &mFpsCounter;
  std::function<void(f32)> mUpdateFn = [](f32) {};
  std::function<void(f32)> mFixedUpdateFn = [](f32) {};
  std::function<void(u32)> mStatsFn = [](u32) {};

  std::function<void()> mRenderFn = []() {};
  std::function<void()> mPrepareFn = []() {};
};
} // namespace quoll
