#include "liquid/core/Base.h"
#include "ActionExecutor.h"

namespace liquid::editor {

ActionExecutor::ActionExecutor(WorkspaceState &state, Path scenePath)
    : mState(state), mScenePath(scenePath),
      mSceneIO(state.assetRegistry, state.scene) {}

void ActionExecutor::execute(const Action &action, std::any data) {
  LIQUID_ASSERT((bool)action.onExecute,
                "Action \"" + String(action.name) + "\" has no executor");
  auto res = action.onExecute(mState, data);

  if (mState.mode == WorkspaceMode::Simulation) {
    return;
  }

  if (!res.entitiesToDelete.empty()) {
    for (auto entity : res.entitiesToDelete) {
      mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath);
    }
  }
}

} // namespace liquid::editor
