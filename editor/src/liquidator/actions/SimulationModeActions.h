#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

static Action StartSimulationModeAction{
    "Start simulation", fa::Play,
    [](WorkspaceState &state, std::any data) {
      state.mode = WorkspaceMode::Simulation;
      state.simulationScene.entityDatabase.destroy();
      state.scene.entityDatabase.duplicate(
          state.simulationScene.entityDatabase);
      state.simulationScene.activeCamera = state.scene.activeCamera;
      state.simulationScene.dummyCamera = state.scene.dummyCamera;
      state.simulationScene.environment = state.scene.environment;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) { return state.mode == WorkspaceMode::Edit; }};

static Action StopSimulationModeAction{
    "Stop simulation", fa::Stop,
    [](WorkspaceState &state, std::any data) {
      state.mode = WorkspaceMode::Edit;
      return ActionExecutorResult{};
    },
    [](WorkspaceState &state) {
      return state.mode == WorkspaceMode::Simulation;
    }};

} // namespace liquid::editor
