#include "quoll/core/Base.h"
#include "EntitySkinnedMeshRendererActions.h"

namespace quoll::editor {

EntitySetSkinnedMeshRendererMaterial::EntitySetSkinnedMeshRendererMaterial(
    Entity entity, usize slot, MaterialAssetHandle handle)
    : mEntity(entity), mSlot(slot), mNewMaterial(handle) {}

ActionExecutorResult
EntitySetSkinnedMeshRendererMaterial::onExecute(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldMaterial = mEntity.get_ref<SkinnedMeshRenderer>()->materials.at(mSlot);
  mEntity.get_ref<SkinnedMeshRenderer>()->materials.at(mSlot) = mNewMaterial;

  ActionExecutorResult result{};
  result.addToHistory = true;
  result.entitiesToSave.push_back(mEntity);
  return result;
}

ActionExecutorResult
EntitySetSkinnedMeshRendererMaterial::onUndo(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.get_ref<SkinnedMeshRenderer>()->materials.at(mSlot) = mOldMaterial;

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntitySetSkinnedMeshRendererMaterial::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!mEntity.has<SkinnedMeshRenderer>()) {
    return false;
  }

  if (mSlot >= mEntity.get_ref<SkinnedMeshRenderer>()->materials.size()) {
    return false;
  }

  return assetRegistry.getMaterials().hasAsset(mNewMaterial);
}

EntityAddSkinnedMeshRendererMaterialSlot::
    EntityAddSkinnedMeshRendererMaterialSlot(Entity entity,
                                             MaterialAssetHandle handle)
    : mEntity(entity), mNewMaterial(handle) {}

ActionExecutorResult EntityAddSkinnedMeshRendererMaterialSlot::onExecute(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.get_ref<SkinnedMeshRenderer>()->materials.push_back(mNewMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityAddSkinnedMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                                 AssetRegistry &assetRegistry) {

  auto &scene = state.scene;

  mEntity.get_ref<SkinnedMeshRenderer>()->materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityAddSkinnedMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!mEntity.has<SkinnedMeshRenderer>()) {
    return false;
  }

  return assetRegistry.getMaterials().hasAsset(mNewMaterial);
}

EntityRemoveLastSkinnedMeshRendererMaterialSlot::
    EntityRemoveLastSkinnedMeshRendererMaterialSlot(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult EntityRemoveLastSkinnedMeshRendererMaterialSlot::onExecute(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldMaterial = mEntity.get_ref<SkinnedMeshRenderer>()->materials.back();

  mEntity.get_ref<SkinnedMeshRenderer>()->materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult EntityRemoveLastSkinnedMeshRendererMaterialSlot::onUndo(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.get_ref<SkinnedMeshRenderer>()->materials.push_back(mOldMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityRemoveLastSkinnedMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!mEntity.has<SkinnedMeshRenderer>()) {
    return false;
  }

  return mEntity.get_ref<SkinnedMeshRenderer>()->materials.size() > 0;
}

} // namespace quoll::editor
