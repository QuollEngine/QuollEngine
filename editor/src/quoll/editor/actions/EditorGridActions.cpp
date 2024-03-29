#include "quoll/core/Base.h"
#include "EditorGridActions.h"

namespace quoll::editor {

bool SetGridLines::isShown(WorkspaceState &state) { return state.grid.x == 1; }

SetGridLines::SetGridLines(bool show) : mShow(show) {}

ActionExecutorResult SetGridLines::onExecute(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  state.grid.x = mShow ? 1 : 0;
  return ActionExecutorResult();
}

bool SetGridLines::predicate(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  return state.grid.x != static_cast<u32>(mShow);
}

bool SetGridAxisLines::isShown(WorkspaceState &state) {
  return state.grid.y == 1;
}

SetGridAxisLines::SetGridAxisLines(bool show) : mShow(show) {}

ActionExecutorResult SetGridAxisLines::onExecute(WorkspaceState &state,
                                                 AssetRegistry &assetRegistry) {
  state.grid.y = mShow ? 1 : 0;
  return ActionExecutorResult();
}

bool SetGridAxisLines::predicate(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) {
  return state.grid.y != static_cast<u32>(mShow);
}

} // namespace quoll::editor
