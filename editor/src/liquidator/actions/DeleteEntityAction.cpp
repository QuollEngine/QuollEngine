#include "liquid/core/Base.h"
#include "DeleteEntityAction.h"

namespace liquid::editor {

DeleteEntity::DeleteEntity(Entity entity) : mEntity(entity) {}

ActionExecutorResult DeleteEntity::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Delete>(mEntity, {});

  auto current = state.selectedEntity;

  bool preserveSelectedEntity = current != mEntity;
  while (preserveSelectedEntity && scene.entityDatabase.has<Parent>(current)) {
    auto parent = scene.entityDatabase.get<Parent>(current).parent;
    preserveSelectedEntity = parent != mEntity;
    current = parent;
  }

  if (!preserveSelectedEntity) {
    state.selectedEntity = Entity::Null;
  }

  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mEntity);
  return res;
}

bool DeleteEntity::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
