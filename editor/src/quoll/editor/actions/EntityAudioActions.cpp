#include "quoll/core/Base.h"
#include "quoll/audio/AudioSource.h"

#include "EntityAudioActions.h"

namespace quoll::editor {

EntitySetAudio::EntitySetAudio(Entity entity, AudioAssetHandle audio)
    : mEntity(entity), mAudio(audio) {}

ActionExecutorResult EntitySetAudio::onExecute(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldAudio = scene.entityDatabase.get<AudioSource>(mEntity).source;
  scene.entityDatabase.set<AudioSource>(mEntity, {mAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetAudio::onUndo(WorkspaceState &state,
                                            AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<AudioSource>(mEntity, {mOldAudio});

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
  return EntityDefaultCreateComponent<AudioSource>(mEntity, {mHandle})
      .onExecute(state, assetRegistry);
}

ActionExecutorResult EntityCreateAudio::onUndo(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  return EntityDefaultCreateComponent<AudioSource>(mEntity, {mHandle})
      .onUndo(state, assetRegistry);
}

bool EntityCreateAudio::predicate(WorkspaceState &state,
                                  AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  return !scene.entityDatabase.has<AudioSource>(mEntity) &&
         assetRegistry.getAudios().hasAsset(mHandle);
}

} // namespace quoll::editor
