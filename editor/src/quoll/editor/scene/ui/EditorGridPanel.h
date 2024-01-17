#pragma once

#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/workspace/WorkspaceState.h"

namespace quoll::editor {

class EditorGridPanel {
public:
  void renderMenu();

  void render(WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  bool mOpen = false;
};

} // namespace quoll::editor
