#include "liquid/core/Base.h"
#include "EntityCollidableActions.h"

namespace liquid::editor {

/**
 * @brief Get defaulty geometry from type
 *
 * @param type Geometry type
 * @return Default geometry parameters
 */
static PhysicsGeometryParams
getDefaultGeometryFromType(const PhysicsGeometryType &type) {
  switch (type) {
  case PhysicsGeometryType::Box:
  default:
    return PhysicsGeometryBox();
  case PhysicsGeometryType::Sphere:
    return PhysicsGeometrySphere();
  case PhysicsGeometryType::Capsule:
    return PhysicsGeometryCapsule();
  case PhysicsGeometryType::Plane:
    return PhysicsGeometryPlane();
  }
}

EntitySetCollidableType::EntitySetCollidableType(Entity entity,
                                                 PhysicsGeometryType type)
    : mEntity(entity), mType(type) {}

ActionExecutorResult EntitySetCollidableType::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  auto &collidable = scene.entityDatabase.get<Collidable>(mEntity);
  collidable.geometryDesc.type = mType;
  collidable.geometryDesc.params = getDefaultGeometryFromType(mType);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCollidableType::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Collidable>(mEntity) &&
         scene.entityDatabase.get<Collidable>(mEntity).geometryDesc.type !=
             mType;
}

EntitySetCollidable::EntitySetCollidable(Entity entity, Collidable collidable)
    : mEntity(entity), mCollidable(collidable) {}

ActionExecutorResult EntitySetCollidable::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Collidable>(mEntity, mCollidable);
  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCollidable::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
