#include "quoll/core/Base.h"
#include "quoll/physics/Collidable.h"
#include "EntityCollidableActions.h"

namespace quoll::editor {

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
  auto &scene = state.scene;

  auto collidable = mEntity.get_ref<Collidable>();
  mOldCollidable = *collidable.get();

  collidable->geometryDesc.type = mType;
  collidable->geometryDesc.params = getDefaultGeometryFromType(mType);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntitySetCollidableType::onUndo(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set(mOldCollidable);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCollidableType::predicate(WorkspaceState &state,
                                        AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return mEntity.has<Collidable>() &&
         mEntity.get_ref<Collidable>()->geometryDesc.type != mType;
}

} // namespace quoll::editor
