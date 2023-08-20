#include "liquid/core/Base.h"
#include "ActionExecutor.h"

namespace liquid::editor {

ActionExecutor::ActionExecutor(WorkspaceState &state,
                               AssetRegistry &assetRegistry, Path scenePath)
    : mState(state), mScenePath(scenePath), mAssetRegistry(assetRegistry),
      mSceneWriter(mState.scene, mAssetRegistry) {}

void ActionExecutor::process() {
  if (!mActionToProcess) {
    return;
  }

  auto action = std::move(mActionToProcess);

  if (!action->predicate(mState, mAssetRegistry)) {
    return;
  }

  auto result = action->onExecute(mState, mAssetRegistry);

  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (result.addToHistory) {
    mUndoStack.push_back(std::move(action));
    mRedoStack.clear();
  }

  saveActionResult(result);
}

void ActionExecutor::execute(std::unique_ptr<Action> action) {
  mActionToProcess = std::move(action);
}

void ActionExecutor::undo() {
  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (mUndoStack.empty()) {
    return;
  }

  auto &action = mUndoStack.back();
  auto result = action->onUndo(mState, mAssetRegistry);
  saveActionResult(result);

  mRedoStack.push_back(std::move(action));
  mUndoStack.pop_back();
}

void ActionExecutor::redo() {
  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (mRedoStack.empty()) {
    return;
  }

  auto &action = mRedoStack.back();
  auto result = action->onExecute(mState, mAssetRegistry);
  saveActionResult(result);

  mUndoStack.push_back(std::move(action));
  mRedoStack.pop_back();
}

void ActionExecutor::saveActionResult(const ActionExecutorResult &result) {
  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (!result.entitiesToDelete.empty()) {
    mSceneWriter.deleteEntities(result.entitiesToDelete);
  }

  if (!result.entitiesToSave.empty()) {
    mSceneWriter.saveEntities(result.entitiesToSave);
  }

  if (result.saveScene) {
    mSceneWriter.saveScene();
  }
}

} // namespace liquid::editor
