#include "liquid/core/Base.h"
#include "TransformScriptingInterface.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

int TransformScriptingInterface::LuaInterface::getPosition(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    // TODO: Print error
    scope.set(nullptr);
    scope.set(nullptr);
    scope.set(nullptr);
    return 3;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  const auto &position =
      entityDatabase.getComponent<LocalTransformComponent>(entity)
          .localPosition;

  scope.set(position.x);
  scope.set(position.y);
  scope.set(position.z);

  return 3;
}

int TransformScriptingInterface::LuaInterface::setPosition(void *state) {
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

  glm::vec3 newPosition;
  newPosition.x = scope.get<float>(2);
  newPosition.y = scope.get<float>(3);
  newPosition.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto &transform =
      entityDatabase.getComponent<LocalTransformComponent>(entity);
  transform.localPosition = newPosition;

  return 0;
}

int TransformScriptingInterface::LuaInterface::getScale(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    // TODO: Print error
    scope.set(nullptr);
    scope.set(nullptr);
    scope.set(nullptr);
    return 3;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  const auto &scale =
      entityDatabase.getComponent<LocalTransformComponent>(entity).localScale;

  scope.set(scale.x);
  scope.set(scale.y);
  scope.set(scale.z);

  return 3;
}

int TransformScriptingInterface::LuaInterface::setScale(void *state) {
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

  glm::vec3 newScale;
  newScale.x = scope.get<float>(2);
  newScale.y = scope.get<float>(3);
  newScale.z = scope.get<float>(4);
  scope.pop(4);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto &transform =
      entityDatabase.getComponent<LocalTransformComponent>(entity);
  transform.localScale = newScale;

  return 0;
}

} // namespace liquid
