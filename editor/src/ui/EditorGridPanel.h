#pragma once

#include "../editor-scene/SceneManager.h"

namespace liquidator {

class EditorGridPanel {
public:
  /**
   * @brief Render editor grid panel
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  bool open = false;
};

} // namespace liquidator
