#include "liquid/core/Base.h"
#include "RigidBodyScriptingInterface.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

int RigidBodyScriptingInterface::LuaInterface::applyForce(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1) || !scope.is<float>(2) || !scope.is<float>(3) ||
      !scope.is<float>(4)) {
    // TODO: Show logs here
    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(1);

  glm::vec3 force;
  force.x = scope.get<float>(2);
  force.y = scope.get<float>(3);
  force.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.setComponent<ForceComponent>(entity, {force});

  return 0;
}

int RigidBodyScriptingInterface::LuaInterface::applyTorque(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1) || !scope.is<float>(2) || !scope.is<float>(3) ||
      !scope.is<float>(4)) {
    // TODO: Show logs here
    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(1);

  glm::vec3 torque;
  torque.x = scope.get<float>(2);
  torque.y = scope.get<float>(3);
  torque.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.setComponent<TorqueComponent>(entity, {torque});

  return 0;
}

} // namespace liquid
