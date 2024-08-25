#include "quoll/core/Base.h"
#include "quoll/scene/Sprite.h"
#include "EntityCreateComponentAction.h"
#include "EntitySpriteActions.h"

namespace quoll::editor {

EntitySetSprite::EntitySetSprite(Entity entity,
                                 AssetHandle<TextureAsset> handle)
    : mEntity(entity), mSprite(handle) {}

ActionExecutorResult EntitySetSprite::onExecute(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldSprite = scene.entityDatabase.get<Sprite>(mEntity).handle;
  scene.entityDatabase.set<Sprite>(mEntity, {mSprite});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetSprite::onUndo(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.set<Sprite>(mEntity, {mOldSprite});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetSprite::predicate(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  return assetRegistry.has(mSprite);
}

EntityCreateSprite::EntityCreateSprite(Entity entity,
                                       AssetHandle<TextureAsset> handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult
EntityCreateSprite::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  return EntityCreateComponent<Sprite>(mEntity, {mHandle})
      .onExecute(state, assetRegistry);
}

ActionExecutorResult EntityCreateSprite::onUndo(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  return EntityCreateComponent<Sprite>(mEntity, {mHandle})
      .onUndo(state, assetRegistry);
}

bool EntityCreateSprite::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  return !scene.entityDatabase.has<Sprite>(mEntity) &&
         assetRegistry.has(mHandle);
}

} // namespace quoll::editor
