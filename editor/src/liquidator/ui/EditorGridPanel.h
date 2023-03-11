#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"

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
   * @param actionExecutor Action executor
   */
  void render(WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  bool mOpen = false;
};

} // namespace liquid::editor
