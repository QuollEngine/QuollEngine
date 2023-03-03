#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/scripting/LuaMessages.h"
#include "liquid/entity/EntityDatabase.h"

#include "RigidBodyScriptingInterface.h"

namespace liquid {

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

} // namespace liquid
