#include "liquid/core/Base.h"
#include "EntitySpriteActions.h"

namespace liquid::editor {

EntitySetSprite::EntitySetSprite(Entity entity, TextureAssetHandle handle)
    : mEntity(entity), mSprite(handle) {}

ActionExecutorResult EntitySetSprite::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldSprite = scene.entityDatabase.get<Sprite>(mEntity).handle;
  scene.entityDatabase.set<Sprite>(mEntity, {mSprite});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetSprite::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Sprite>(mEntity, {mOldSprite});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetSprite::predicate(WorkspaceState &state) {
  return state.assetRegistry.getTextures().hasAsset(mSprite);
}

EntityCreateSprite::EntityCreateSprite(Entity entity, TextureAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntityCreateSprite::onExecute(WorkspaceState &state) {
  return EntityDefaultCreateComponent<Sprite>(mEntity, {mHandle})
      .onExecute(state);
}

ActionExecutorResult EntityCreateSprite::onUndo(WorkspaceState &state) {
  return EntityDefaultCreateComponent<Sprite>(mEntity, {mHandle}).onUndo(state);
}

bool EntityCreateSprite::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  return !scene.entityDatabase.has<Sprite>(mEntity) &&
         state.assetRegistry.getTextures().hasAsset(mHandle);
}
} // namespace liquid::editor