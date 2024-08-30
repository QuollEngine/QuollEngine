#include "quoll/core/Base.h"
#include "ActionExecutor.h"

namespace quoll::editor {

ActionExecutor::ActionExecutor(WorkspaceState &state, AssetCache &assetCache)
    : mState(state), mAssetCache(assetCache) {}

void ActionExecutor::setAssetSyncer(AssetSyncer *assetSyncer) {
  mAssetSyncer = assetSyncer;
}

void ActionExecutor::process() {
  if (!mActionToProcess) {
    return;
  }

  auto action = std::move(mActionToProcess);

  if (!action->predicate(mState, mAssetCache)) {
    return;
  }

  auto result = action->onExecute(mState, mAssetCache);

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
  if (mUndoStack.empty()) {
    return;
  }

  auto &action = mUndoStack.back();
  auto result = action->onUndo(mState, mAssetCache);
  saveActionResult(result);

  mRedoStack.push_back(std::move(action));
  mUndoStack.pop_back();
}

void ActionExecutor::redo() {
  if (mRedoStack.empty()) {
    return;
  }

  auto &action = mRedoStack.back();
  auto result = action->onExecute(mState, mAssetCache);
  saveActionResult(result);

  mUndoStack.push_back(std::move(action));
  mRedoStack.pop_back();
}

void ActionExecutor::saveActionResult(const ActionExecutorResult &result) {
  if (!mAssetSyncer) {
    return;
  }

  if (!result.entitiesToDelete.empty()) {
    mAssetSyncer->deleteEntities(result.entitiesToDelete);
  }

  if (!result.entitiesToSave.empty()) {
    mAssetSyncer->syncEntities(result.entitiesToSave);
  }

  if (result.saveScene) {
    mAssetSyncer->syncScene();
  }
}

} // namespace quoll::editor
