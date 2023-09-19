#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaScope.h"
#include "quoll/scripting/LuaMessages.h"
#include "quoll/scripting/ComponentLuaInterfaceCommon.h"
#include "quoll/physics/PhysicsSystem.h"

#include "CollidableScriptingInterface.h"

namespace quoll {

int CollidableScriptingInterface::LuaInterface::setDefaultMaterial(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_default_material");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    entityDatabase.set(entity, quoll::Collidable{});
  } else {
    entityDatabase.get<Collidable>(entity).materialDesc =
        quoll::PhysicsMaterialDesc{};
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::getStaticFriction(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_static_friction");
    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    return 1;
  }

  float staticFriction =
      entityDatabase.get<Collidable>(entity).materialDesc.staticFriction;
  scope.set(staticFriction);

  return 1;
}

int CollidableScriptingInterface::LuaInterface::setStaticFriction(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_static_friction");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<float>(
        getName(), "set_static_friction");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float staticFriction = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.materialDesc.staticFriction = staticFriction;
    entityDatabase.set(entity, collidable);
  } else {
    entityDatabase.get<Collidable>(entity).materialDesc.staticFriction =
        staticFriction;
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::getDynamicFriction(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_dynamic_friction");
    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    return 1;
  }

  float dynamicFriction =
      entityDatabase.get<Collidable>(entity).materialDesc.dynamicFriction;
  scope.set(dynamicFriction);

  return 1;
}

int CollidableScriptingInterface::LuaInterface::setDynamicFriction(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_dynamic_friction");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<float>(
        getName(), "set_dynamic_friction");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float dynamicFriction = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.materialDesc.dynamicFriction = dynamicFriction;
    entityDatabase.set(entity, collidable);
  } else {
    entityDatabase.get<Collidable>(entity).materialDesc.dynamicFriction =
        dynamicFriction;
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::getRestitution(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_restitution");
    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    return 1;
  }

  float restitution =
      entityDatabase.get<Collidable>(entity).materialDesc.restitution;
  scope.set(restitution);

  return 1;
}

int CollidableScriptingInterface::LuaInterface::setRestitution(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_restitution");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_restitution");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float restitution = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.materialDesc.restitution = restitution;
    entityDatabase.set(entity, collidable);
  } else {
    entityDatabase.get<Collidable>(entity).materialDesc.restitution =
        restitution;
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::setBoxGeometry(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_box_geometry");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float>(
               getName(), "set_box_geometry");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec3 halfExtents;
  halfExtents.x = scope.get<float>(2);
  halfExtents.y = scope.get<float>(3);
  halfExtents.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Box;
    collidable.geometryDesc.params = PhysicsGeometryBox{halfExtents};
    entityDatabase.set(entity, collidable);
  } else {
    auto &collidable = entityDatabase.get<Collidable>(entity);
    collidable.geometryDesc.type = PhysicsGeometryType::Box;
    collidable.geometryDesc.params = PhysicsGeometryBox{halfExtents};
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::setSphereGeometry(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_sphere_geometry");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<float>(
        getName(), "set_sphere_geometry");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float radius = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Sphere;
    collidable.geometryDesc.params = PhysicsGeometrySphere{radius};
    entityDatabase.set(entity, collidable);
  } else {
    auto &collidable = entityDatabase.get<Collidable>(entity);
    collidable.geometryDesc.type = PhysicsGeometryType::Sphere;
    collidable.geometryDesc.params = PhysicsGeometrySphere{radius};
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::setCapsuleGeometry(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_capsule_geometry");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float>(getName(),
                                                       "set_capsule_geometry");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float radius = scope.get<float>(2);
  float halfHeight = scope.get<float>(3);
  scope.pop(3);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Capsule;
    collidable.geometryDesc.params = PhysicsGeometryCapsule{radius, halfHeight};
    entityDatabase.set(entity, collidable);
  } else {
    auto &collidable = entityDatabase.get<Collidable>(entity);
    collidable.geometryDesc.type = PhysicsGeometryType::Capsule;
    collidable.geometryDesc.params = PhysicsGeometryCapsule{radius, halfHeight};
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::setPlaneGeometry(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_plane_geometry");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<Collidable>(entity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Plane;
    collidable.geometryDesc.params = PhysicsGeometryPlane{};

    entityDatabase.set(entity, collidable);
  } else {
    auto &collidable = entityDatabase.get<Collidable>(entity);
    collidable.geometryDesc.type = PhysicsGeometryType::Plane;
    collidable.geometryDesc.params = PhysicsGeometryPlane{};
  }

  return 0;
}

int CollidableScriptingInterface::LuaInterface::sweep(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "sweep");
    scope.set(false);
    return 1;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4) ||
      !scope.is<float>(5)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float, float>(getName(),
                                                                     "sweep");
    scope.set(false);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float dirX = scope.get<float>(2);
  float dirY = scope.get<float>(3);
  float dirZ = scope.get<float>(4);
  float distance = scope.get<float>(5);
  scope.pop(3);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  PhysicsSystem &physicsSystem = *static_cast<PhysicsSystem *>(
      scope.getGlobal<LuaUserData>("__privatePhysics").pointer);

  scope.set(physicsSystem.sweep(entityDatabase, entity, {dirX, dirY, dirZ},
                                distance));

  return 1;
}

int CollidableScriptingInterface::LuaInterface::deleteThis(void *state) {
  return ComponentLuaInterfaceCommon::deleteComponent<Collidable>(getName(),
                                                                  state);
}

} // namespace quoll
