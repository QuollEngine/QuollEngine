#pragma once

#include "liquidator/state/WorkspaceState.h"

namespace liquid::editor {

/**
 * @brief Editor grid panel component
 */
class EditorGridPanel {
public:
  /**
   * @brief Render editor grid panel
   *
   * @param state Workspace state
   */
  void render(WorkspaceState &state);

private:
  bool mOpen = false;
};

} // namespace liquid::editor
