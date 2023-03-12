#include "liquid/core/Base.h"
#include "EntityRigidBodyActions.h"

namespace liquid::editor {

EntitySetRigidBody::EntitySetRigidBody(Entity entity, RigidBody rigidBody)
    : mEntity(entity), mRigidBody(rigidBody) {}

ActionExecutorResult EntitySetRigidBody::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mRigidBody);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetRigidBody::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
