#include "liquid/core/Base.h"
#include "SceneActions.h"

namespace liquid::editor {

SceneSetStartingCamera::SceneSetStartingCamera(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult SceneSetStartingCamera::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.activeCamera = mEntity;

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetStartingCamera::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.activeCamera != mEntity &&
         scene.entityDatabase.has<PerspectiveLens>(mEntity);
}

} // namespace liquid::editor
