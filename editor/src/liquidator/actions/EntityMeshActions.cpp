#include "liquid/core/Base.h"
#include "EntityMeshActions.h"

namespace liquid::editor {

EntitySetMesh::EntitySetMesh(Entity entity, MeshAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntitySetMesh::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Mesh>(mEntity, {mHandle});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetMesh::predicate(WorkspaceState &state) {
  return state.assetRegistry.getMeshes().hasAsset(mHandle);
}

EntitySetSkinnedMesh::EntitySetSkinnedMesh(Entity entity,
                                           SkinnedMeshAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntitySetSkinnedMesh::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<SkinnedMesh>(mEntity, {mHandle});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetSkinnedMesh::predicate(WorkspaceState &state) {
  return state.assetRegistry.getSkinnedMeshes().hasAsset(mHandle);
}

} // namespace liquid::editor
