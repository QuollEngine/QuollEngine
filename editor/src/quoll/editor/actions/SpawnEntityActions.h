#pragma once

#include "quoll/scene/PrefabAsset.h"
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
  SpawnPrefabAtView(AssetRef<PrefabAsset> prefab, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  AssetRef<PrefabAsset> mPrefab;
  Entity mCamera;
  Entity mSpawnedRootEntity = Entity::Null;
};

class SpawnSpriteAtView : public Action {
public:
  SpawnSpriteAtView(AssetRef<TextureAsset> texture, Entity camera);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  AssetRef<TextureAsset> mTexture;
  Entity mCamera;
  Entity mSpawnedEntity = Entity::Null;
};

} // namespace quoll::editor
