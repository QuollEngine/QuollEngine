#include "quoll/core/Base.h"
#include "HistoryActions.h"

namespace quoll::editor {

Undo::Undo(ActionExecutor &actionExecutor) : mActionExecutor(actionExecutor) {}

ActionExecutorResult Undo::onExecute(WorkspaceState &state,
                                     AssetCache &assetCache) {
  mActionExecutor.undo();
  return {};
}

bool Undo::predicate(WorkspaceState &state, AssetCache &assetCache) {
  return !mActionExecutor.getUndoStack().empty();
}

Redo::Redo(ActionExecutor &actionExecutor) : mActionExecutor(actionExecutor) {}

ActionExecutorResult Redo::onExecute(WorkspaceState &state,
                                     AssetCache &assetCache) {
  mActionExecutor.redo();
  return {};
}

bool Redo::predicate(WorkspaceState &state, AssetCache &assetCache) {
  return !mActionExecutor.getRedoStack().empty();
}

} // namespace quoll::editor
