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
  SpawnPrefabAtView(AssetHandle<PrefabAsset> handle, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  AssetHandle<PrefabAsset> mHandle;
  Entity mCamera;
  Entity mSpawnedRootEntity = Entity::Null;
};

class SpawnSpriteAtView : public Action {
public:
  SpawnSpriteAtView(AssetHandle<TextureAsset> handle, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  AssetHandle<TextureAsset> mHandle;
  Entity mCamera;
  Entity mSpawnedEntity = Entity::Null;
};

} // namespace quoll::editor
