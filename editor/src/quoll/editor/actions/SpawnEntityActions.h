#pragma once

#include "Action.h"

namespace quoll::editor {

class SpawnEmptyEntityAtView : public Action {
public:
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mSpawnedEntity = Entity::Null;
};

class SpawnPrefabAtView : public Action {
public:
  SpawnPrefabAtView(AssetHandle<PrefabAsset> handle, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  AssetHandle<PrefabAsset> mHandle;
  Entity mCamera;
  Entity mSpawnedRootEntity = Entity::Null;
};

class SpawnSpriteAtView : public Action {
public:
  SpawnSpriteAtView(AssetHandle<TextureAsset> handle, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  AssetHandle<TextureAsset> mHandle;
  Entity mCamera;
  Entity mSpawnedEntity = Entity::Null;
};

} // namespace quoll::editor
