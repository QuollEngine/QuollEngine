#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetMesh : public Action {
public:
  EntitySetMesh(Entity entity, AssetHandle<MeshAsset> mesh);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<MeshAsset> mMesh;
  AssetHandle<MeshAsset> mOldMesh;
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
  AssetHandle<MeshAsset> mOldMesh;
};

} // namespace quoll::editor
