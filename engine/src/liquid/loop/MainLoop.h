#pragma once

#include "liquid/renderer/render-graph/RenderGraph.h"

namespace liquid {

class Window;
class StatsManager;

class MainLoop {
public:
  /**
   * @brief Create main loop
   *
   * @param window Window
   * @param statsManager Stats manager
   */
  MainLoop(Window &window, StatsManager &statsManager);

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
  StatsManager &mStatsManager;
  std::function<bool(float)> mUpdateFn;
  std::function<void()> mRenderFn;
};
} // namespace liquid
