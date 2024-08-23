#include "quoll/core/Base.h"
#include "quoll/audio/AudioSource.h"
#include "EntityAudioActions.h"
#include "EntityCreateComponentAction.h"

namespace quoll::editor {

EntitySetAudio::EntitySetAudio(Entity entity, AudioAssetHandle audio)
    : mEntity(entity), mAudio(audio) {}

ActionExecutorResult EntitySetAudio::onExecute(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldAudio = mEntity.get_ref<AudioSource>()->source;
  mEntity.set<AudioSource>({mAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetAudio::onUndo(WorkspaceState &state,
                                            AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set<AudioSource>({mOldAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetAudio::predicate(WorkspaceState &state,
                               AssetRegistry &assetRegistry) {
  return assetRegistry.getAudios().hasAsset(mAudio);
}

EntityCreateAudio::EntityCreateAudio(Entity entity, AudioAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult
EntityCreateAudio::onExecute(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  return EntityCreateComponent<AudioSource>(mEntity, {mHandle})
      .onExecute(state, assetRegistry);
}

ActionExecutorResult EntityCreateAudio::onUndo(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  return EntityCreateComponent<AudioSource>(mEntity, {mHandle})
      .onUndo(state, assetRegistry);
}

bool EntityCreateAudio::predicate(WorkspaceState &state,
                                  AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  return !mEntity.has<AudioSource>() &&
         assetRegistry.getAudios().hasAsset(mHandle);
}

} // namespace quoll::editor
