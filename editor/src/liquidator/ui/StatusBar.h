#pragma once

#include "liquidator/editor-scene/EditorCamera.h"

namespace liquid::editor {

/**
 * @brief Status bar component
 */
class StatusBar {
public:
  /**
   * @brief Render status bar
   *
   * @param editorCamera Editor camera
   */
  static void render(EditorCamera &editorCamera);
};

} // namespace liquid::editor
