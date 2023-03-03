#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/scripting/LuaMessages.h"
#include "liquid/entity/EntityDatabase.h"

#include "TransformScriptingInterface.h"

namespace liquid {

int TransformScriptingInterface::LuaInterface::getPosition(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_position");

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

  const auto &position =
      entityDatabase.get<LocalTransform>(entity).localPosition;

  scope.set(position.x);
  scope.set(position.y);
  scope.set(position.z);

  return 3;
}

int TransformScriptingInterface::LuaInterface::setPosition(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_position");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float>(getName(),
                                                              "set_position");
    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec3 newPosition;
  newPosition.x = scope.get<float>(2);
  newPosition.y = scope.get<float>(3);
  newPosition.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto &transform = entityDatabase.get<LocalTransform>(entity);
  transform.localPosition = newPosition;

  return 0;
}

int TransformScriptingInterface::LuaInterface::getScale(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_scale");
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

  const auto &scale = entityDatabase.get<LocalTransform>(entity).localScale;

  scope.set(scale.x);
  scope.set(scale.y);
  scope.set(scale.z);

  return 3;
}

int TransformScriptingInterface::LuaInterface::setScale(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_scale");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3) || !scope.is<float>(4)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float, float>(getName(),
                                                              "set_scale");
    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec3 newScale;
  newScale.x = scope.get<float>(2);
  newScale.y = scope.get<float>(3);
  newScale.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto &transform = entityDatabase.get<LocalTransform>(entity);
  transform.localScale = newScale;

  return 0;
}

} // namespace liquid
