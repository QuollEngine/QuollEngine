#include "quoll/core/Base.h"
#include "EntityMeshRendererActions.h"

namespace quoll::editor {

EntitySetMeshRendererMaterial::EntitySetMeshRendererMaterial(
    Entity entity, usize slot, AssetRef<MaterialAsset> material)
    : mEntity(entity), mSlot(slot), mNewMaterial(material) {}

ActionExecutorResult
EntitySetMeshRendererMaterial::onExecute(WorkspaceState &state,
                                         AssetCache &assetCache) {
  auto &scene = state.scene;

  mOldMaterial =
      scene.entityDatabase.get<MeshRenderer>(mEntity).materials.at(mSlot);
  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.at(mSlot) =
      mNewMaterial.handle();

  ActionExecutorResult result{};
  result.addToHistory = true;
  result.entitiesToSave.push_back(mEntity);
  return result;
}

ActionExecutorResult
EntitySetMeshRendererMaterial::onUndo(WorkspaceState &state,
                                      AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.at(mSlot) =
      mOldMaterial;

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntitySetMeshRendererMaterial::predicate(WorkspaceState &state,
                                              AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<MeshRenderer>(mEntity)) {
    return false;
  }

  if (mSlot >=
      scene.entityDatabase.get<MeshRenderer>(mEntity).materials.size()) {
    return false;
  }

  return mNewMaterial;
}

EntityAddMeshRendererMaterialSlot::EntityAddMeshRendererMaterialSlot(
    Entity entity, AssetRef<MaterialAsset> material)
    : mEntity(entity), mNewMaterial(material) {}

ActionExecutorResult
EntityAddMeshRendererMaterialSlot::onExecute(WorkspaceState &state,
                                             AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.push_back(
      mNewMaterial.handle());

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  result.addToHistory = true;
  return result;
}

ActionExecutorResult
EntityAddMeshRendererMaterialSlot::onUndo(WorkspaceState &state,
                                          AssetCache &assetCache) {

  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.pop_back();

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityAddMeshRendererMaterialSlot::predicate(WorkspaceState &state,
                                                  AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<MeshRenderer>(mEntity)) {
    return false;
  }

  return mNewMaterial;
}

EntityRemoveLastMeshRendererMaterialSlot::
    EntityRemoveLastMeshRendererMaterialSlot(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityRemoveLastMeshRendererMaterialSlot::onExecute(WorkspaceState &state,
                                                    AssetCache &assetCache) {
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
                                                 AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.get<MeshRenderer>(mEntity).materials.push_back(
      mOldMaterial);

  ActionExecutorResult result{};
  result.entitiesToSave.push_back(mEntity);
  return result;
}

bool EntityRemoveLastMeshRendererMaterialSlot::predicate(
    WorkspaceState &state, AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<MeshRenderer>(mEntity)) {
    return false;
  }

  return scene.entityDatabase.get<MeshRenderer>(mEntity).materials.size() > 0;
}

} // namespace quoll::editor
