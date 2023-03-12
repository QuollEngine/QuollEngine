#include "liquid/core/Base.h"
#include "EntityAudioActions.h"

namespace liquid::editor {

EntitySetAudio::EntitySetAudio(Entity entity, AudioAssetHandle audio)
    : mEntity(entity), mAudio(audio) {}

ActionExecutorResult EntitySetAudio::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<AudioSource>(mEntity, {mAudio});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetAudio::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
