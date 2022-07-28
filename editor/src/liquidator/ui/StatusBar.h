#pragma once

#include "liquidator/editor-scene/EditorManager.h"

namespace liquidator {

/**
 * @brief Status bar component
 */
class StatusBar {
public:
  /**
   * @brief Render status bar
   *
   * @param editorManager Editor manager
   */
  void render(EditorManager &editorManager);
};

} // namespace liquidator
