#pragma once

#include "liquidator/editor-scene/EditorManager.h"

namespace liquid::editor {

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
  static void render(EditorManager &editorManager);
};

} // namespace liquid::editor
