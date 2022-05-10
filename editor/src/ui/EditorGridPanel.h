#pragma once

#include "../editor-scene/SceneManager.h"

namespace liquidator {

/**
 * @brief Editor grid panel component
 */
class EditorGridPanel {
public:
  /**
   * @brief Render editor grid panel
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  bool mOpen = false;
};

} // namespace liquidator
