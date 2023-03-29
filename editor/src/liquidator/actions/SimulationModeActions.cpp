#include "liquid/core/Base.h"
#include "SimulationModeActions.h"

namespace liquid::editor {

ActionExecutorResult StartSimulationMode::onExecute(WorkspaceState &state) {
  state.mode = WorkspaceMode::Simulation;
  state.simulationScene.entityDatabase.destroy();
  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);
  state.simulationScene.dummyCamera = state.scene.dummyCamera;
  state.simulationScene.environment = state.scene.environment;

  if (state.scene.entityDatabase.has<Camera>(state.scene.activeCamera)) {
    state.simulationScene.activeCamera = state.scene.activeCamera;
    state.activeCamera = state.scene.activeCamera;
  } else {
    state.simulationScene.activeCamera = state.scene.dummyCamera;
    state.activeCamera = state.scene.dummyCamera;
  }

  return ActionExecutorResult{};
}

bool StartSimulationMode::predicate(WorkspaceState &state) {
  return state.mode == WorkspaceMode::Edit;
}

ActionExecutorResult StopSimulationMode::onExecute(WorkspaceState &state) {
  state.mode = WorkspaceMode::Edit;
  state.activeCamera = state.camera;
  return ActionExecutorResult{};
}

bool StopSimulationMode::predicate(WorkspaceState &state) {
  return state.mode == WorkspaceMode::Simulation;
}

} // namespace liquid::editor
