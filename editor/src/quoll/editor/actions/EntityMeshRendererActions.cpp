#include "quoll/core/Base.h"
#include "EntityMeshRendererActions.h"

namespace quoll::editor {

EntitySetMeshRendererMaterial::EntitySetMeshRendererMaterial(
    Entity entity, usize slot, AssetHandle<MaterialAsset> handle)
    : mEntity(entity), mSlot(slot), mNewMaterial(handle) {}

ActionExecutorResult
EntitySetMeshRendererMaterial::onExecute(WorkspaceState &state,
                                         AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldMaterial =
      scene.entityDatabase.get<MeshRenderer>(mEntity).materials.at(mSlot);
  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.at(mSlot) =
      mNewMaterial;

  ActionExecutorResult result{};
  result.addToHistory = true;
  result.entitiesToSave.push_back(mEntity);
  return result;
}

ActionExecutorResult
EntitySetMeshRendererMaterial::onUndo(WorkspaceState &state,
                                      AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.at(mSlot) =
      mOldMaterial;

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntitySetMeshRendererMaterial::predicate(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<MeshRenderer>(mEntity)) {
    return false;
  }

  if (mSlot >=
      scene.entityDatabase.get<MeshRenderer>(mEntity).materials.size()) {
    return false;
  }

  return assetRegistry.getMaterials().hasAsset(mNewMaterial);
}

EntityAddMeshRendererMaterialSlot::EntityAddMeshRendererMaterialSlot(
    Entity entity, AssetHandle<MaterialAsset> handle)
    : mEntity(entity), mNewMaterial(handle) {}

ActionExecutorResult
EntityAddMeshRendererMaterialSlot::onExecute(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.push_back(
      mNewMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityAddMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                          AssetRegistry &assetRegistry) {

  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityAddMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<MeshRenderer>(mEntity)) {
    return false;
  }

  return assetRegistry.getMaterials().hasAsset(mNewMaterial);
}

EntityRemoveLastMeshRendererMaterialSlot::
    EntityRemoveLastMeshRendererMaterialSlot(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult EntityRemoveLastMeshRendererMaterialSlot::onExecute(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldMaterial =
      scene.entityDatabase.get<MeshRenderer>(mEntity).materials.back();

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityRemoveLastMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                                 AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.push_back(
      mOldMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityRemoveLastMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<MeshRenderer>(mEntity)) {
    return false;
  }

  return scene.entityDatabase.get<MeshRenderer>(mEntity).materials.size() > 0;
}

} // namespace quoll::editor
