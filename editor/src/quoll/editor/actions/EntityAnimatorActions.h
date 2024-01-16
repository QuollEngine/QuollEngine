#pragma once

#include "Action.h"

namespace quoll::editor {

class EntityCreateAnimator : public Action {
public:
  EntityCreateAnimator(Entity entity, AnimatorAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AnimatorAssetHandle mHandle;
};

class EntitySetAnimator : public Action {
public:
  EntitySetAnimator(Entity entity, AnimatorAssetHandle script);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AnimatorAssetHandle mAnimator;
  AnimatorAssetHandle mOldAnimator{};
};

} // namespace quoll::editor
