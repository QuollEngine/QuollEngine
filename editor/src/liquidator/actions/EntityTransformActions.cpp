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
  if (!scene.entityDatabase.has<WorldTransform>(mEntity)) {
    scene.entityDatabase.set<WorldTransform>(mEntity, {});
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetLocalTransform::predicate(WorkspaceState &state) { return true; }

EntitySetLocalTransformContinuous::EntitySetLocalTransformContinuous(
    Entity entity, LocalTransform localTransformStart)
    : mEntity(entity), mLocalTransformStart(localTransformStart) {}

void EntitySetLocalTransformContinuous::setLocalTransformFinal(
    LocalTransform localTransformFinal) {
  mLocalTransformFinal = localTransformFinal;
}

ActionExecutorResult
EntitySetLocalTransformContinuous::onExecute(WorkspaceState &state) {
  return EntitySetLocalTransform(mEntity, mLocalTransformFinal.value())
      .onExecute(state);
}

bool EntitySetLocalTransformContinuous::predicate(WorkspaceState &state) {
  return mLocalTransformFinal.has_value();
}

} // namespace liquid::editor
