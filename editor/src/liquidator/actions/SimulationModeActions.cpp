#include "liquid/core/Base.h"
#include "SimulationModeActions.h"

namespace liquid::editor {

ActionExecutorResult
StartSimulationModeAction::onExecute(WorkspaceState &state) {
  state.mode = WorkspaceMode::Simulation;
  state.simulationScene.entityDatabase.destroy();
  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);
  state.simulationScene.activeCamera = state.scene.activeCamera;
  state.simulationScene.dummyCamera = state.scene.dummyCamera;
  state.simulationScene.environment = state.scene.environment;
  return ActionExecutorResult{};
}

bool StartSimulationModeAction::predicate(WorkspaceState &state) {
  return state.mode == WorkspaceMode::Edit;
}

ActionExecutorResult
StopSimulationModeAction::onExecute(WorkspaceState &state) {
  state.mode = WorkspaceMode::Edit;
  return ActionExecutorResult{};
}

bool StopSimulationModeAction::predicate(WorkspaceState &state) {
  return state.mode == WorkspaceMode::Simulation;
}

} // namespace liquid::editor
