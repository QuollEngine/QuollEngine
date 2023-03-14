#include "liquid/core/Base.h"
#include "ActionExecutor.h"

namespace liquid::editor {

ActionExecutor::ActionExecutor(WorkspaceState &state, Path scenePath)
    : mState(state), mScenePath(scenePath),
      mSceneIO(state.assetRegistry, state.scene) {}

void ActionExecutor::execute(const std::unique_ptr<Action> &action) {
  auto res = action->onExecute(mState);

  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (!res.entitiesToDelete.empty()) {
    for (auto entity : res.entitiesToDelete) {
      mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath);
    }
  }

  if (!res.entitiesToSave.empty()) {
    for (auto entity : res.entitiesToSave) {
      mSceneIO.saveEntity(entity, mScenePath);
    }
  }
}

} // namespace liquid::editor
