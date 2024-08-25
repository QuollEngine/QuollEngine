#pragma once

#include "Action.h"

namespace quoll::editor {

class EntityCreateAnimator : public Action {
public:
  EntityCreateAnimator(Entity entity, AssetHandle<AnimatorAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<AnimatorAsset> mHandle;
};

class EntitySetAnimator : public Action {
public:
  EntitySetAnimator(Entity entity, AssetHandle<AnimatorAsset> script);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<AnimatorAsset> mAnimator;
  AssetHandle<AnimatorAsset> mOldAnimator;
};

} // namespace quoll::editor
