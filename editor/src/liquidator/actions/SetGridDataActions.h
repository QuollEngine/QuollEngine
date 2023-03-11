#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

static Action SetGridLinesAction{
    "Toggle grid lines", "",
    [](WorkspaceState &state, std::any data) {
      state.grid.x = std::any_cast<bool>(data) ? 1 : 0;

      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) { return state.grid.x == 1; }};

static Action SetGridAxisLinesAction{
    "Toggle axis lines", "",
    [](WorkspaceState &state, std::any data) {
      state.grid.y = std::any_cast<bool>(data) ? 1 : 0;

      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) { return state.grid.y == 1; }};

} // namespace liquid::editor
