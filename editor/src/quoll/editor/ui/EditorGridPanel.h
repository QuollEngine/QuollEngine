#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"

namespace quoll::editor {

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

} // namespace quoll::editor
