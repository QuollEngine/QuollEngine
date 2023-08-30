#include "quoll/core/Base.h"
#include "TransformOperationActions.h"

namespace quoll::editor {

SetActiveTransform::SetActiveTransform(TransformOperation transformOperation)
    : mTransformOperation(transformOperation) {}

ActionExecutorResult
SetActiveTransform::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  mOldTransformOperation = state.activeTransform;
  state.activeTransform = mTransformOperation;

  ActionExecutorResult res{};
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SetActiveTransform::onUndo(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  state.activeTransform = mOldTransformOperation;
  return ActionExecutorResult{};
}

bool SetActiveTransform::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  return state.activeTransform != mTransformOperation;
}

} // namespace quoll::editor
