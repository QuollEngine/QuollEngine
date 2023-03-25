#include "liquid/core/Base.h"
#include "TransformOperationActions.h"

namespace liquid::editor {

SetActiveTransform::SetActiveTransform(TransformOperation transformOperation)
    : mTransformOperation(transformOperation) {}

ActionExecutorResult SetActiveTransform::onExecute(WorkspaceState &state) {
  state.activeTransform = mTransformOperation;
  return ActionExecutorResult{};
}

bool SetActiveTransform::predicate(WorkspaceState &state) {
  return state.activeTransform != mTransformOperation;
}

} // namespace liquid::editor
