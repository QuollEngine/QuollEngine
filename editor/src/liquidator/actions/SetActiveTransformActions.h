#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

static const Action SetActiveTransformToMoveAction{
    "SetActiveTransformToMove", fa::Arrows,
    [](WorkspaceState &state, std::any data) {
      state.activeTransform = TransformOperation::Move;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.activeTransform == TransformOperation::Move;
    }};

static const Action SetActiveTransformToRotateAction{
    "SetActiveTransformToRotate", fa::Rotate,
    [](WorkspaceState &state, std::any data) {
      state.activeTransform = TransformOperation::Rotate;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.activeTransform == TransformOperation::Rotate;
    }};

static const Action SetActiveTransformToScaleAction{
    "SetActiveTransformToScale", fa::ExpandAlt,
    [](WorkspaceState &state, std::any data) {
      state.activeTransform = TransformOperation::Scale;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.activeTransform == TransformOperation::Scale;
    }};

} // namespace liquid::editor
