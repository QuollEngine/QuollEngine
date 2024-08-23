#include "quoll/core/Base.h"
#include "EntityMeshRendererActions.h"

namespace quoll::editor {

EntitySetMeshRendererMaterial::EntitySetMeshRendererMaterial(
    Entity entity, usize slot, MaterialAssetHandle handle)
    : mEntity(entity), mSlot(slot), mNewMaterial(handle) {}

ActionExecutorResult
EntitySetMeshRendererMaterial::onExecute(WorkspaceState &state,
                                         AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldMaterial = mEntity.get_ref<MeshRenderer>()->materials.at(mSlot);
  mEntity.get_ref<MeshRenderer>()->materials.at(mSlot) = mNewMaterial;

  ActionExecutorResult result{};
  result.addToHistory = true;
  result.entitiesToSave.push_back(mEntity);
  return result;
}

ActionExecutorResult
EntitySetMeshRendererMaterial::onUndo(WorkspaceState &state,
                                      AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.get_ref<MeshRenderer>()->materials.at(mSlot) = mOldMaterial;

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntitySetMeshRendererMaterial::predicate(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!mEntity.has<MeshRenderer>()) {
    return false;
  }

  if (mSlot >= mEntity.get_ref<MeshRenderer>()->materials.size()) {
    return false;
  }

  return assetRegistry.getMaterials().hasAsset(mNewMaterial);
}

EntityAddMeshRendererMaterialSlot::EntityAddMeshRendererMaterialSlot(
    Entity entity, MaterialAssetHandle handle)
    : mEntity(entity), mNewMaterial(handle) {}

ActionExecutorResult
EntityAddMeshRendererMaterialSlot::onExecute(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.get_ref<MeshRenderer>()->materials.push_back(mNewMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityAddMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                          AssetRegistry &assetRegistry) {

  auto &scene = state.scene;

  mEntity.get_ref<MeshRenderer>()->materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityAddMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!mEntity.has<MeshRenderer>()) {
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

  mOldMaterial = mEntity.get_ref<MeshRenderer>()->materials.back();

  mEntity.get_ref<MeshRenderer>()->materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityRemoveLastMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                                 AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.get_ref<MeshRenderer>()->materials.push_back(mOldMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityRemoveLastMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!mEntity.has<MeshRenderer>()) {
    return false;
  }

  return mEntity.get_ref<MeshRenderer>()->materials.size() > 0;
}

} // namespace quoll::editor
