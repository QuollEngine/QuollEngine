#include "quoll/core/Base.h"
#include "EntitySkinnedMeshRendererActions.h"

namespace quoll::editor {

EntitySetSkinnedMeshRendererMaterial::EntitySetSkinnedMeshRendererMaterial(
    Entity entity, usize slot, AssetRef<MaterialAsset> material)
    : mEntity(entity), mSlot(slot), mNewMaterial(material) {}

ActionExecutorResult
EntitySetSkinnedMeshRendererMaterial::onExecute(WorkspaceState &state,
                                                AssetCache &assetCache) {
  auto &scene = state.scene;

  mOldMaterial =
      scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.at(
          mSlot);
  scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.at(mSlot) =
      mNewMaterial;

  ActionExecutorResult result{};
  result.addToHistory = true;
  result.entitiesToSave.push_back(mEntity);
  return result;
}

ActionExecutorResult
EntitySetSkinnedMeshRendererMaterial::onUndo(WorkspaceState &state,
                                             AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.at(mSlot) =
      mOldMaterial;

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntitySetSkinnedMeshRendererMaterial::predicate(WorkspaceState &state,
                                                     AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<SkinnedMeshRenderer>(mEntity)) {
    return false;
  }

  if (mSlot >=
      scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.size()) {
    return false;
  }

  return mNewMaterial;
}

EntityAddSkinnedMeshRendererMaterialSlot::
    EntityAddSkinnedMeshRendererMaterialSlot(Entity entity,
                                             AssetRef<MaterialAsset> material)
    : mEntity(entity), mNewMaterial(material) {}

ActionExecutorResult
EntityAddSkinnedMeshRendererMaterialSlot::onExecute(WorkspaceState &state,
                                                    AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.push_back(
      mNewMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityAddSkinnedMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                                 AssetCache &assetCache) {

  auto &scene = state.scene;

  scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityAddSkinnedMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<SkinnedMeshRenderer>(mEntity)) {
    return false;
  }

  return mNewMaterial;
}

EntityRemoveLastSkinnedMeshRendererMaterialSlot::
    EntityRemoveLastSkinnedMeshRendererMaterialSlot(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult EntityRemoveLastSkinnedMeshRendererMaterialSlot::onExecute(
    WorkspaceState &state, AssetCache &assetCache) {
  auto &scene = state.scene;

  mOldMaterial =
      scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.back();

  scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult EntityRemoveLastSkinnedMeshRendererMaterialSlot::onUndo(
    WorkspaceState &state, AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity).materials.push_back(
      mOldMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityRemoveLastSkinnedMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<SkinnedMeshRenderer>(mEntity)) {
    return false;
  }

  return scene.entityDatabase.get<SkinnedMeshRenderer>(mEntity)
             .materials.size() > 0;
}

} // namespace quoll::editor
