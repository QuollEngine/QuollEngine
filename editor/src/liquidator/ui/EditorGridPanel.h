#pragma once

#include "liquidator/editor-scene/EditorManager.h"

namespace liquid::editor {

/**
 * @brief Editor grid panel component
 */
class EditorGridPanel {
public:
  /**
   * @brief Render editor grid panel
   *
   * @param editorManager Editor manager
   */
  void render(EditorManager &editorManager);

private:
  bool mOpen = false;
};

} // namespace liquid::editor
