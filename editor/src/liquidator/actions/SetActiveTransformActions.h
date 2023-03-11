#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

static const Action SetActiveTransformToMoveAction{
    "SetActiveTransformToMove",
    [](WorkspaceState &state, std::any data) {
      state.activeTransform = TransformOperation::Move;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.activeTransform == TransformOperation::Move;
    }};

static const Action SetActiveTransformToRotateAction{
    "SetActiveTransformToRotate",
    [](WorkspaceState &state, std::any data) {
      state.activeTransform = TransformOperation::Rotate;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.activeTransform == TransformOperation::Rotate;
    }};

static const Action SetActiveTransformToScaleAction{
    "SetActiveTransformToScale",
    [](WorkspaceState &state, std::any data) {
      state.activeTransform = TransformOperation::Scale;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.activeTransform == TransformOperation::Scale;
    }};

} // namespace liquid::editor
