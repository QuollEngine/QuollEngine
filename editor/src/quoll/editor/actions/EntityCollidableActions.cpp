#include "quoll/core/Base.h"
#include "EntityCollidableActions.h"

namespace quoll::editor {

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

ActionExecutorResult
EntitySetCollidableType::onExecute(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  auto &collidable = scene.entityDatabase.get<Collidable>(mEntity);
  mOldCollidable = collidable;

  collidable.geometryDesc.type = mType;
  collidable.geometryDesc.params = getDefaultGeometryFromType(mType);

  scene.entityDatabase.set(mEntity, collidable);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntitySetCollidableType::onUndo(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mOldCollidable);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCollidableType::predicate(WorkspaceState &state,
                                        AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Collidable>(mEntity) &&
         scene.entityDatabase.get<Collidable>(mEntity).geometryDesc.type !=
             mType;
}

} // namespace quoll::editor
