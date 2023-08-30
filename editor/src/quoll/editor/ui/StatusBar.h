#pragma once

#include "quoll/editor/editor-scene/EditorCamera.h"

namespace quoll::editor {

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

} // namespace quoll::editor
