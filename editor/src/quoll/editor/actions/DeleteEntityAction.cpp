#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/scene/Parent.h"

#include "DeleteEntityAction.h"

namespace quoll::editor {

DeleteEntity::DeleteEntity(Entity entity) : mEntity(entity) {}

ActionExecutorResult DeleteEntity::onExecute(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

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

  bool preserveActiveEnvironment = true;
  {
    auto current = scene.activeEnvironment;

    preserveActiveEnvironment = current != mEntity;
    while (preserveActiveEnvironment &&
           scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveActiveEnvironment = parent != mEntity;
      current = parent;
    }
  }

  if (!preserveStartingCamera) {
    scene.activeCamera = scene.dummyCamera;
  }

  if (!preserveActiveCamera) {
    state.activeCamera = state.camera;
  }

  if (!preserveActiveEnvironment) {
    scene.activeEnvironment = scene.dummyEnvironment;
  }

  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mEntity);
  res.saveScene = !preserveStartingCamera || !preserveActiveEnvironment;
  return res;
}

bool DeleteEntity::predicate(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  return true;
}

} // namespace quoll::editor
