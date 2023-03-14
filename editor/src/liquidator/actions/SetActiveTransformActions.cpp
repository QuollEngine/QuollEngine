#include "liquid/core/Base.h"
#include "SetActiveTransformActions.h"

namespace liquid::editor {

ActionExecutorResult
SetActiveTransformToMoveAction::onExecute(WorkspaceState &state) {
  state.activeTransform = TransformOperation::Move;
  return ActionExecutorResult{};
}

bool SetActiveTransformToMoveAction::predicate(WorkspaceState &state) {
  return state.activeTransform == TransformOperation::Move;
}

ActionExecutorResult
SetActiveTransformToRotateAction::onExecute(WorkspaceState &state) {
  state.activeTransform = TransformOperation::Rotate;
  return ActionExecutorResult{};
}

bool SetActiveTransformToRotateAction::predicate(WorkspaceState &state) {
  return state.activeTransform == TransformOperation::Rotate;
}

ActionExecutorResult
SetActiveTransformToScaleAction::onExecute(WorkspaceState &state) {
  state.activeTransform = TransformOperation::Scale;
  return ActionExecutorResult{};
}

bool SetActiveTransformToScaleAction::predicate(WorkspaceState &state) {
  return state.activeTransform == TransformOperation::Scale;
}

} // namespace liquid::editor
