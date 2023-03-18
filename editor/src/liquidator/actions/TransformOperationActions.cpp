#include "liquid/core/Base.h"
#include "TransformOperationActions.h"

namespace liquid::editor {

SetActiveTransformAction::SetActiveTransformAction(
    TransformOperation transformOperation)
    : mTransformOperation(transformOperation) {}

ActionExecutorResult
SetActiveTransformAction::onExecute(WorkspaceState &state) {
  state.activeTransform = mTransformOperation;
  return ActionExecutorResult{};
}

bool SetActiveTransformAction::predicate(WorkspaceState &state) {
  return state.activeTransform != mTransformOperation;
}

} // namespace liquid::editor
