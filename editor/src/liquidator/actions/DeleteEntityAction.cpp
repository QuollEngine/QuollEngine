#include "liquid/core/Base.h"
#include "DeleteEntityAction.h"

namespace liquid::editor {

DeleteEntityAction::DeleteEntityAction(Entity entity) : mEntity(entity) {}

ActionExecutorResult DeleteEntityAction::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Delete>(mEntity, {});

  if (state.selectedEntity == mEntity) {
    state.selectedEntity = Entity::Null;
  }

  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mEntity);
  return res;
}

bool DeleteEntityAction::predicate(WorkspaceState &state) { return false; }

} // namespace liquid::editor
