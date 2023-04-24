#include "liquid/core/Base.h"
#include "TransformOperationActions.h"

namespace liquid::editor {

SetActiveTransform::SetActiveTransform(TransformOperation transformOperation)
    : mTransformOperation(transformOperation) {}

ActionExecutorResult SetActiveTransform::onExecute(WorkspaceState &state) {
  mOldTransformOperation = state.activeTransform;
  state.activeTransform = mTransformOperation;

  ActionExecutorResult res{};
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SetActiveTransform::onUndo(WorkspaceState &state) {
  state.activeTransform = mOldTransformOperation;
  return ActionExecutorResult{};
}

bool SetActiveTransform::predicate(WorkspaceState &state) {
  return state.activeTransform != mTransformOperation;
}

} // namespace liquid::editor
