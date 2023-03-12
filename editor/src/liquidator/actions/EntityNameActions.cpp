#include "liquid/core/Base.h"
#include "EntityNameActions.h"

namespace liquid::editor {

EntitySetName::EntitySetName(Entity entity, Name name)
    : mEntity(entity), mName(name) {}

ActionExecutorResult EntitySetName::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mName);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetName::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
