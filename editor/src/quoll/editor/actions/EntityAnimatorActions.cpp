#include "quoll/core/Base.h"
#include "EntityAnimatorActions.h"
#include "EntityCreateComponentAction.h"

namespace quoll::editor {

EntityCreateAnimator::EntityCreateAnimator(Entity entity,
                                           AnimatorAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult
EntityCreateAnimator::onExecute(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  return EntityCreateComponent<Animator>(mEntity, {mHandle})
      .onExecute(state, assetRegistry);
}

ActionExecutorResult
EntityCreateAnimator::onUndo(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  return EntityCreateComponent<Animator>(mEntity, {mHandle})
      .onUndo(state, assetRegistry);
}

bool EntityCreateAnimator::predicate(WorkspaceState &state,
                                     AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  return !mEntity.has<Animator>() &&
         assetRegistry.getAnimators().hasAsset(mHandle);
}

EntitySetAnimator::EntitySetAnimator(Entity entity, AnimatorAssetHandle script)
    : mEntity(entity), mAnimator(script) {}

ActionExecutorResult
EntitySetAnimator::onExecute(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldAnimator = mEntity.get_ref<Animator>()->asset;

  mEntity.set<Animator>({mAnimator});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult EntitySetAnimator::onUndo(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set<Animator>({mOldAnimator});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetAnimator::predicate(WorkspaceState &state,
                                  AssetRegistry &assetRegistry) {
  return assetRegistry.getAnimators().hasAsset(mAnimator);
}

} // namespace quoll::editor
