#include "quoll/core/Base.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/Parent.h"

#include "SimulationModeActions.h"

namespace quoll::editor {

ActionExecutorResult
StartSimulationMode::onExecute(WorkspaceState &state,
                               AssetRegistry &assetRegistry) {
  state.mode = WorkspaceMode::Simulation;
  state.simulationScene.entityDatabase.destroy();
  state.scene.entityDatabase.duplicate(state.simulationScene.entityDatabase);
  state.simulationScene.dummyCamera = state.scene.dummyCamera;
  state.simulationScene.activeEnvironment = state.scene.activeEnvironment;
  state.simulationScene.dummyEnvironment = state.scene.dummyEnvironment;

  if (state.scene.entityDatabase.has<Camera>(state.scene.activeCamera)) {
    state.simulationScene.activeCamera = state.scene.activeCamera;
    state.activeCamera = state.scene.activeCamera;
  } else {
    state.simulationScene.activeCamera = state.scene.dummyCamera;
    state.activeCamera = state.scene.dummyCamera;
  }

  return ActionExecutorResult{};
}

bool StartSimulationMode::predicate(WorkspaceState &state,
                                    AssetRegistry &assetRegistry) {
  return state.mode == WorkspaceMode::Edit;
}

ActionExecutorResult
StopSimulationMode::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  state.mode = WorkspaceMode::Edit;
  state.activeCamera = state.camera;

  bool preserveSelectedEntity = true;
  {
    auto current = state.selectedEntity;

    preserveSelectedEntity = state.scene.entityDatabase.exists(current);
    while (preserveSelectedEntity &&
           state.scene.entityDatabase.has<Parent>(current)) {
      auto parent = state.scene.entityDatabase.get<Parent>(current).parent;
      preserveSelectedEntity = state.scene.entityDatabase.exists(parent);
      current = parent;
    }
  }

  if (!preserveSelectedEntity) {
    state.selectedEntity = Entity::Null;
  }

  return ActionExecutorResult{};
}

bool StopSimulationMode::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  return state.mode == WorkspaceMode::Simulation;
}

} // namespace quoll::editor
