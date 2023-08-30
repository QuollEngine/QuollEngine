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
  /**
   * @brief Create main loop
   *
   * @param window Window
   * @param fpsCounter FPS counter
   */
  MainLoop(Window &window, FPSCounter &fpsCounter);

  /**
   * @brief Run main loop
   */
  void run();

  /**
   * @brief Set update function
   *
   * @param updateFn Update function
   */
  void setUpdateFn(const std::function<bool(float)> &updateFn);

  /**
   * @brief Set render function
   *
   * @param renderFn Render function
   */
  void setRenderFn(const std::function<void()> &renderFn);

private:
  Window &mWindow;
  FPSCounter &mFpsCounter;
  std::function<bool(float)> mUpdateFn;
  std::function<void()> mRenderFn;
};
} // namespace quoll
