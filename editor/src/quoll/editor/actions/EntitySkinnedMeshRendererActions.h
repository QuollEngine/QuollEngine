#pragma once

#include "quoll/renderer/SkinnedMeshRenderer.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetSkinnedMeshRendererMaterial : public Action {
public:
  EntitySetSkinnedMeshRendererMaterial(Entity entity, usize slot,
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

class EntityAddSkinnedMeshRendererMaterialSlot : public Action {
public:
  EntityAddSkinnedMeshRendererMaterialSlot(Entity entity,
                                           MaterialAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  MaterialAssetHandle mNewMaterial;
};

class EntityRemoveLastSkinnedMeshRendererMaterialSlot : public Action {
public:
  EntityRemoveLastSkinnedMeshRendererMaterialSlot(Entity entity);

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
