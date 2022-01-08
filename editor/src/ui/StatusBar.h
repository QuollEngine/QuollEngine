#pragma once

#include "../editor-scene/SceneManager.h"

namespace liquidator {

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
