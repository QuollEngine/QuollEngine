#pragma once

#include "../editor-scene/SceneManager.h"

namespace liquidator {

/**
 * @brief Status bar component
 */
class StatusBar {
public:
  /**
   * @brief Render status bar
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);
};

} // namespace liquidator
