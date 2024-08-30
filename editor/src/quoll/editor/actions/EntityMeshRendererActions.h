#pragma once

#include "quoll/renderer/MeshRenderer.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetMeshRendererMaterial : public Action {
public:
  EntitySetMeshRendererMaterial(Entity entity, usize slot,
                                AssetHandle<MaterialAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  usize mSlot;
  AssetHandle<MaterialAsset> mOldMaterial;
  AssetHandle<MaterialAsset> mNewMaterial;
};

class EntityAddMeshRendererMaterialSlot : public Action {
public:
  EntityAddMeshRendererMaterialSlot(Entity entity,
                                    AssetHandle<MaterialAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetHandle<MaterialAsset> mNewMaterial;
};

class EntityRemoveLastMeshRendererMaterialSlot : public Action {
public:
  EntityRemoveLastMeshRendererMaterialSlot(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetHandle<MaterialAsset> mOldMaterial;
};

} // namespace quoll::editor
