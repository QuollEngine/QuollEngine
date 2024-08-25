#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetSprite : public Action {
public:
  EntitySetSprite(Entity entity, AssetHandle<TextureAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<TextureAsset> mSprite;
  AssetHandle<TextureAsset> mOldSprite;
};

class EntityCreateSprite : public Action {
public:
  EntityCreateSprite(Entity entity, AssetHandle<TextureAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<TextureAsset> mHandle;
};

} // namespace quoll::editor
