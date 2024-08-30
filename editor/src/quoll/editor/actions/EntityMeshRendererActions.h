#pragma once

#include "quoll/renderer/MeshRenderer.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetMeshRendererMaterial : public Action {
public:
  EntitySetMeshRendererMaterial(Entity entity, usize slot,
                                AssetRef<MaterialAsset> material);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  usize mSlot;
  AssetHandle<MaterialAsset> mOldMaterial;
  AssetRef<MaterialAsset> mNewMaterial;
};

class EntityAddMeshRendererMaterialSlot : public Action {
public:
  EntityAddMeshRendererMaterialSlot(Entity entity,
                                    AssetRef<MaterialAsset> material);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetRef<MaterialAsset> mNewMaterial;
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
