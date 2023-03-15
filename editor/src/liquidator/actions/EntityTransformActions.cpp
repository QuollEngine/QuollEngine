#include "liquid/core/Base.h"
#include "EntityTransformActions.h"

namespace liquid::editor {

EntitySetLocalTransform::EntitySetLocalTransform(Entity entity,
                                                 LocalTransform localTransform)
    : mEntity(entity), mLocalTransform(localTransform) {}

ActionExecutorResult EntitySetLocalTransform::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mLocalTransform);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetLocalTransform::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
