#include "liquid/core/Base.h"
#include "ActionExecutor.h"

namespace liquid::editor {

ActionExecutor::ActionExecutor(WorkspaceState &state, Path scenePath)
    : mState(state), mScenePath(scenePath),
      mSceneIO(state.assetRegistry, state.scene) {}

void ActionExecutor::process() {
  if (!mActionToProcess) {
    return;
  }

  auto action = std::move(mActionToProcess);

  if (!action->predicate(mState)) {
    return;
  }

  auto result = action->onExecute(mState);

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
  auto result = action->onUndo(mState);
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
  auto result = action->onExecute(mState);
  saveActionResult(result);

  mUndoStack.push_back(std::move(action));
  mRedoStack.pop_back();
}

void ActionExecutor::saveActionResult(const ActionExecutorResult &result) {
  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (!result.entitiesToDelete.empty()) {
    for (auto entity : result.entitiesToDelete) {
      mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath);
    }
  }

  if (!result.entitiesToSave.empty()) {
    for (auto entity : result.entitiesToSave) {
      mSceneIO.saveEntity(entity, mScenePath);
    }
  }

  if (result.saveScene) {
    mSceneIO.saveStartingCamera(mScenePath);
    mSceneIO.saveEnvironment(mScenePath);
  }
}

} // namespace liquid::editor
