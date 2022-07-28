#pragma once

#include "liquidator/editor-scene/EditorManager.h"

namespace liquidator {

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

} // namespace liquidator
