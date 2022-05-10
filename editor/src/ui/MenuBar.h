#pragma once

#include "../editor-scene/SceneManager.h"

namespace liquidator {

class MenuBar {
public:
  /**
   * @brief Render menu bar
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);
};

} // namespace liquidator
