#include "liquid/core/Base.h"
#include "HistoryActions.h"

namespace liquid::editor {

Undo::Undo(ActionExecutor &actionExecutor) : mActionExecutor(actionExecutor) {}

ActionExecutorResult Undo::onExecute(WorkspaceState &state) {
  mActionExecutor.undo();
  return {};
}

bool Undo::predicate(WorkspaceState &state) {
  return !mActionExecutor.getUndoStack().empty();
}

Redo::Redo(ActionExecutor &actionExecutor) : mActionExecutor(actionExecutor) {}

ActionExecutorResult Redo::onExecute(WorkspaceState &state) {
  mActionExecutor.redo();
  return {};
}

bool Redo::predicate(WorkspaceState &state) {
  return !mActionExecutor.getRedoStack().empty();
}

} // namespace liquid::editor
