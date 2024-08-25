#include "quoll/core/Base.h"
#include "EntityAnimatorActions.h"
#include "EntityCreateComponentAction.h"

namespace quoll::editor {

EntityCreateAnimator::EntityCreateAnimator(Entity entity,
                                           AssetHandle<AnimatorAsset> handle)
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
  return !scene.entityDatabase.has<Animator>(mEntity) &&
         assetRegistry.has(mHandle);
}

EntitySetAnimator::EntitySetAnimator(Entity entity,
                                     AssetHandle<AnimatorAsset> script)
    : mEntity(entity), mAnimator(script) {}

ActionExecutorResult
EntitySetAnimator::onExecute(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldAnimator = scene.entityDatabase.get<Animator>(mEntity).asset;

  scene.entityDatabase.set<Animator>(mEntity, {mAnimator});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult EntitySetAnimator::onUndo(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.set<Animator>(mEntity, {mOldAnimator});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetAnimator::predicate(WorkspaceState &state,
                                  AssetRegistry &assetRegistry) {
  return assetRegistry.has(mAnimator);
}

} // namespace quoll::editor
