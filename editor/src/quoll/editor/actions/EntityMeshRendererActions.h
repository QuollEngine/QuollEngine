#pragma once

#include "quoll/renderer/MeshRenderer.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetMeshRendererMaterial : public Action {
public:
  EntitySetMeshRendererMaterial(Entity entity, usize slot,
                                MaterialAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  usize mSlot;
  MaterialAssetHandle mOldMaterial = MaterialAssetHandle::Null;
  MaterialAssetHandle mNewMaterial;
};

class EntityAddMeshRendererMaterialSlot : public Action {
public:
  EntityAddMeshRendererMaterialSlot(Entity entity, MaterialAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  MaterialAssetHandle mNewMaterial;
};

class EntityRemoveLastMeshRendererMaterialSlot : public Action {
public:
  EntityRemoveLastMeshRendererMaterialSlot(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  MaterialAssetHandle mOldMaterial = MaterialAssetHandle::Null;
};

} // namespace quoll::editor
