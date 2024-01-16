#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetMesh : public Action {
public:
  EntitySetMesh(Entity entity, MeshAssetHandle mesh);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  MeshAssetHandle mMesh;
  MeshAssetHandle mOldMesh = MeshAssetHandle::Null;
};

class EntityDeleteMesh : public Action {
public:
  EntityDeleteMesh(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  MeshAssetHandle mOldMesh = MeshAssetHandle::Null;
};

} // namespace quoll::editor
