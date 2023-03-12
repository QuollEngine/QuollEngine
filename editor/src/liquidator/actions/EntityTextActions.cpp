#include "liquid/core/Base.h"
#include "EntityTransformActions.h"
#include "EntityTextActions.h"

namespace liquid::editor {

EntitySetText::EntitySetText(Entity entity, Text text)
    : mEntity(entity), mText(text) {}

ActionExecutorResult EntitySetText::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mText);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetText::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
