#include "quoll/core/Base.h"
#include "quoll/audio/AudioSource.h"
#include "EntityAudioActions.h"
#include "EntityCreateComponentAction.h"

namespace quoll::editor {

EntitySetAudio::EntitySetAudio(Entity entity, AssetHandle<AudioAsset> audio)
    : mEntity(entity), mAudio(audio) {}

ActionExecutorResult EntitySetAudio::onExecute(WorkspaceState &state,
                                               AssetCache &assetCache) {
  auto &scene = state.scene;

  mOldAudio = scene.entityDatabase.get<AudioSource>(mEntity).source;
  scene.entityDatabase.set<AudioSource>(mEntity, {mAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetAudio::onUndo(WorkspaceState &state,
                                            AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.set<AudioSource>(mEntity, {mOldAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetAudio::predicate(WorkspaceState &state, AssetCache &assetCache) {
  return assetCache.getRegistry().has(mAudio);
}

EntityCreateAudio::EntityCreateAudio(Entity entity,
                                     AssetHandle<AudioAsset> handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntityCreateAudio::onExecute(WorkspaceState &state,
                                                  AssetCache &assetCache) {
  return EntityCreateComponent<AudioSource>(mEntity, {mHandle})
      .onExecute(state, assetCache);
}

ActionExecutorResult EntityCreateAudio::onUndo(WorkspaceState &state,
                                               AssetCache &assetCache) {
  return EntityCreateComponent<AudioSource>(mEntity, {mHandle})
      .onUndo(state, assetCache);
}

bool EntityCreateAudio::predicate(WorkspaceState &state,
                                  AssetCache &assetCache) {
  auto &scene = state.scene;
  return !scene.entityDatabase.has<AudioSource>(mEntity) &&
         assetCache.getRegistry().has(mHandle);
}

} // namespace quoll::editor
