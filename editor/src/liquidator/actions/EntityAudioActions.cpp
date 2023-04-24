#include "liquid/core/Base.h"
#include "EntityAudioActions.h"

namespace liquid::editor {

EntitySetAudio::EntitySetAudio(Entity entity, AudioAssetHandle audio)
    : mEntity(entity), mAudio(audio) {}

ActionExecutorResult EntitySetAudio::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldAudio = scene.entityDatabase.get<AudioSource>(mEntity).source;
  scene.entityDatabase.set<AudioSource>(mEntity, {mAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

ActionExecutorResult EntitySetAudio::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<AudioSource>(mEntity, {mOldAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetAudio::predicate(WorkspaceState &state) {
  return state.assetRegistry.getAudios().hasAsset(mAudio);
}

EntityCreateAudio::EntityCreateAudio(Entity entity, AudioAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntityCreateAudio::onExecute(WorkspaceState &state) {
  return EntityDefaultCreateComponent<AudioSource>(mEntity, {mHandle})
      .onExecute(state);
}

ActionExecutorResult EntityCreateAudio::onUndo(WorkspaceState &state) {
  return EntityDefaultCreateComponent<AudioSource>(mEntity, {mHandle})
      .onUndo(state);
}

bool EntityCreateAudio::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  return !scene.entityDatabase.has<AudioSource>(mEntity) &&
         state.assetRegistry.getAudios().hasAsset(mHandle);
}

} // namespace liquid::editor
