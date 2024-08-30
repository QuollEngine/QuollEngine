#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetSprite : public Action {
public:
  EntitySetSprite(Entity entity, AssetHandle<TextureAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetHandle<TextureAsset> mSprite;
  AssetHandle<TextureAsset> mOldSprite;
};

class EntityCreateSprite : public Action {
public:
  EntityCreateSprite(Entity entity, AssetHandle<TextureAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetHandle<TextureAsset> mHandle;
};

} // namespace quoll::editor
