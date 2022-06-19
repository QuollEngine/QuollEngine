#include "liquid/core/Base.h"
#include "LocalTransformComponent.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

int LocalTransformComponent::LuaInterface::setPosition(void *state) {
  LuaScope scope(state);

  if (!scope.is<float>(1) || !scope.is<float>(2) || !scope.is<float>(3)) {
    // TODO: Show logs here
    return 0;
  }

  glm::vec3 newPosition;
  newPosition.x = scope.get<float>(1);
  newPosition.y = scope.get<float>(2);
  newPosition.z = scope.get<float>(3);
  scope.pop(3);

  EntityContext &entityContext = *static_cast<EntityContext *>(
      scope.getGlobal<LuaUserData>("__privateContext").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  auto &transform = entityContext.getComponent<LocalTransformComponent>(entity);
  transform.localPosition = newPosition;

  return 0;
}

int LocalTransformComponent::LuaInterface::getScale(void *state) {
  LuaScope scope(state);
  EntityContext &entityContext = *static_cast<EntityContext *>(
      scope.getGlobal<LuaUserData>("__privateContext").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  const auto &scale =
      entityContext.getComponent<LocalTransformComponent>(entity).localScale;

  scope.set(scale.x);
  scope.set(scale.y);
  scope.set(scale.z);

  return 3;
}

int LocalTransformComponent::LuaInterface::setScale(void *state) {
  LuaScope scope(state);

  if (!scope.is<float>(1) || !scope.is<float>(2) || !scope.is<float>(3)) {
    // TODO: Show logs here
    return 0;
  }

  glm::vec3 newScale;
  newScale.x = scope.get<float>(1);
  newScale.y = scope.get<float>(2);
  newScale.z = scope.get<float>(3);
  scope.pop(3);

  EntityContext &entityContext = *static_cast<EntityContext *>(
      scope.getGlobal<LuaUserData>("__privateContext").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  auto &transform = entityContext.getComponent<LocalTransformComponent>(entity);
  transform.localScale = newScale;

  return 0;
}

int LocalTransformComponent::LuaInterface::getPosition(void *state) {
  LuaScope scope(state);
  EntityContext &entityContext = *static_cast<EntityContext *>(
      scope.getGlobal<LuaUserData>("__privateContext").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  const auto &position =
      entityContext.getComponent<LocalTransformComponent>(entity).localPosition;

  scope.set(position.x);
  scope.set(position.y);
  scope.set(position.z);

  return 3;
}

} // namespace liquid
