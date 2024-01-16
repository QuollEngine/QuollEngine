#pragma once

#include "Action.h"

namespace quoll::editor {

class SpawnEmptyEntityAtView : public Action {
public:
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mSpawnedEntity = Entity::Null;
};

class SpawnPrefabAtView : public Action {
public:
  SpawnPrefabAtView(PrefabAssetHandle handle, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  PrefabAssetHandle mHandle;
  Entity mCamera;
  Entity mSpawnedRootEntity = Entity::Null;
};

class SpawnSpriteAtView : public Action {
public:
  SpawnSpriteAtView(TextureAssetHandle handle, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  TextureAssetHandle mHandle;
  Entity mCamera;
  Entity mSpawnedEntity = Entity::Null;
};

} // namespace quoll::editor
