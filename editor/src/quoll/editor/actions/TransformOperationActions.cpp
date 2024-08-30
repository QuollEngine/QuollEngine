#include "quoll/core/Base.h"
#include "TransformOperationActions.h"

namespace quoll::editor {

SetActiveTransform::SetActiveTransform(TransformOperation transformOperation)
    : mTransformOperation(transformOperation) {}

ActionExecutorResult SetActiveTransform::onExecute(WorkspaceState &state,
                                                   AssetCache &assetCache) {
  mOldTransformOperation = state.activeTransform;
  state.activeTransform = mTransformOperation;

  ActionExecutorResult res{};
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SetActiveTransform::onUndo(WorkspaceState &state,
                                                AssetCache &assetCache) {
  state.activeTransform = mOldTransformOperation;
  return ActionExecutorResult{};
}

bool SetActiveTransform::predicate(WorkspaceState &state,
                                   AssetCache &assetCache) {
  return state.activeTransform != mTransformOperation;
}

} // namespace quoll::editor
