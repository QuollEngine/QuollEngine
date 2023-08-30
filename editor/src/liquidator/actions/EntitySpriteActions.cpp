#include "quoll/core/Base.h"
#include "EntitySpriteActions.h"

namespace quoll::editor {

EntitySetSprite::EntitySetSprite(Entity entity, TextureAssetHandle handle)
    : mEntity(entity), mSprite(handle) {}

ActionExecutorResult EntitySetSprite::onExecute(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldSprite = scene.entityDatabase.get<Sprite>(mEntity).handle;
  scene.entityDatabase.set<Sprite>(mEntity, {mSprite});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetSprite::onUndo(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Sprite>(mEntity, {mOldSprite});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetSprite::predicate(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  return assetRegistry.getTextures().hasAsset(mSprite);
}

EntityCreateSprite::EntityCreateSprite(Entity entity, TextureAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult
EntityCreateSprite::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  return EntityDefaultCreateComponent<Sprite>(mEntity, {mHandle})
      .onExecute(state, assetRegistry);
}

ActionExecutorResult EntityCreateSprite::onUndo(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  return EntityDefaultCreateComponent<Sprite>(mEntity, {mHandle})
      .onUndo(state, assetRegistry);
}

bool EntityCreateSprite::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  return !scene.entityDatabase.has<Sprite>(mEntity) &&
         assetRegistry.getTextures().hasAsset(mHandle);
}

} // namespace quoll::editor
