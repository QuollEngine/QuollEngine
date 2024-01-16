#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetSprite : public Action {
public:
  EntitySetSprite(Entity entity, TextureAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  TextureAssetHandle mSprite;
  TextureAssetHandle mOldSprite = TextureAssetHandle::Null;
};

class EntityCreateSprite : public Action {
public:
  EntityCreateSprite(Entity entity, TextureAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  TextureAssetHandle mHandle;
};

} // namespace quoll::editor
