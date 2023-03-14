#include "liquid/core/Base.h"
#include "SetGridDataActions.h"

namespace liquid::editor {

bool SetGridLinesAction::isShown(WorkspaceState &state) {
  return state.grid.x == 1;
}

SetGridLinesAction::SetGridLinesAction(bool show) : mShow(show) {}

ActionExecutorResult SetGridLinesAction::onExecute(WorkspaceState &state) {
  state.grid.x = mShow ? 1 : 0;
  return ActionExecutorResult();
}

bool SetGridLinesAction::predicate(WorkspaceState &state) {
  return state.grid.x != static_cast<uint32_t>(mShow);
}

bool SetGridAxisLinesAction::isShown(WorkspaceState &state) {
  return state.grid.y == 1;
}

SetGridAxisLinesAction::SetGridAxisLinesAction(bool show) : mShow(show) {}

ActionExecutorResult SetGridAxisLinesAction::onExecute(WorkspaceState &state) {
  state.grid.y = mShow ? 1 : 0;
  return ActionExecutorResult();
}

bool SetGridAxisLinesAction::predicate(WorkspaceState &state) {
  return state.grid.y != static_cast<uint32_t>(mShow);
}

} // namespace liquid::editor
