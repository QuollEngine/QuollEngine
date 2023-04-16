#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/entity/EntityDatabase.h"
#include "liquid/scripting/LuaScope.h"
#include "liquid/scripting/LuaMessages.h"
#include "liquid/scripting/ComponentLuaInterfaceCommon.h"

#include "RigidBodyScriptingInterface.h"

namespace liquid {

int RigidBodyScriptingInterface::LuaInterface::setDefaultParams(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_default_params");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.set<RigidBody>(entity, {});

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::getMass(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_mass");
    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<RigidBody>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    return 1;
  }

  float mass = entityDatabase.get<RigidBody>(entity).dynamicDesc.mass;
  scope.set(mass);

  return 1;
}

int RigidBodyScriptingInterface::LuaInterface::setMass(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_mass");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_mass");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float mass = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<RigidBody>(entity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.mass = mass;
    entityDatabase.set(entity, rigidBody);
  } else {
    entityDatabase.get<RigidBody>(entity).dynamicDesc.mass = mass;
  }

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::getInertia(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_inertia");
    scope.set(nullptr);
    scope.set(nullptr);
    scope.set(nullptr);
    return 3;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<RigidBody>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    scope.set(nullptr);
    scope.set(nullptr);
    return 3;
  }

  const auto &inertia =
      entityDatabase.get<RigidBody>(entity).dynamicDesc.inertia;

  scope.set(inertia.x);
  scope.set(inertia.y);
  scope.set(inertia.z);

  return 3;
}

int RigidBodyScriptingInterface::LuaInterface::setInertia(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_inertia");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float>(getName(),
                                                              "set_inertia");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec3 inertia;
  inertia.x = scope.get<float>(2);
  inertia.y = scope.get<float>(3);
  inertia.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<RigidBody>(entity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.inertia = inertia;
    entityDatabase.set(entity, rigidBody);
  } else {
    entityDatabase.get<RigidBody>(entity).dynamicDesc.inertia = inertia;
  }

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::isGravityApplied(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "is_gravity_applied");
    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<RigidBody>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    return 1;
  }

  bool applyGravity =
      entityDatabase.get<RigidBody>(entity).dynamicDesc.applyGravity;
  scope.set(applyGravity);

  return 1;
}

int RigidBodyScriptingInterface::LuaInterface::applyGravity(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "apply_gravity");

    return 0;
  }

  if (!scope.is<bool>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<bool>(getName(), "apply_gravity");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  bool apply = scope.get<bool>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<RigidBody>(entity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.applyGravity = apply;
    entityDatabase.set(entity, rigidBody);
  } else {
    entityDatabase.get<RigidBody>(entity).dynamicDesc.applyGravity = apply;
  }

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::applyForce(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "apply_force");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float>(getName(),
                                                              "apply_force");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec3 force;
  force.x = scope.get<float>(2);
  force.y = scope.get<float>(3);
  force.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.set<Force>(entity, {force});

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::applyTorque(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "apply_torque");
    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float>(getName(),
                                                              "apply_torque");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec3 torque;
  torque.x = scope.get<float>(2);
  torque.y = scope.get<float>(3);
  torque.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.set<Torque>(entity, {torque});

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::clear(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "clear");
    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.set<RigidBodyClear>(entity, {});

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::deleteThis(void *state) {
  return ComponentLuaInterfaceCommon::deleteComponent<RigidBody>(getName(),
                                                                 state);
}

} // namespace liquid
