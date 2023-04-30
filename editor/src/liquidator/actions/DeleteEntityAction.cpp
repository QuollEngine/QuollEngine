#include "liquid/core/Base.h"
#include "DeleteEntityAction.h"

namespace liquid::editor {

DeleteEntity::DeleteEntity(Entity entity) : mEntity(entity) {}

ActionExecutorResult DeleteEntity::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Delete>(mEntity, {});

  bool preserveSelectedEntity = true;
  {
    auto current = state.selectedEntity;

    preserveSelectedEntity = current != mEntity;
    while (preserveSelectedEntity &&
           scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveSelectedEntity = parent != mEntity;
      current = parent;
    }
  }

  if (!preserveSelectedEntity) {
    state.selectedEntity = Entity::Null;
  }

  bool preserveStartingCamera = true;
  {
    auto current = scene.activeCamera;

    preserveStartingCamera = current != mEntity;
    while (preserveStartingCamera &&
           scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveStartingCamera = parent != mEntity;
      current = parent;
    }
  }

  bool preserveActiveCamera = true;
  {
    auto current = state.activeCamera;

    preserveActiveCamera = current != mEntity;
    while (preserveActiveCamera && scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveActiveCamera = parent != mEntity;
      current = parent;
    }
  }

  if (!preserveStartingCamera) {
    scene.activeCamera = scene.dummyCamera;
  }

  if (!preserveActiveCamera) {
    state.activeCamera = state.camera;
  }

  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mEntity);
  res.saveScene = !preserveStartingCamera;
  return res;
}

bool DeleteEntity::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
