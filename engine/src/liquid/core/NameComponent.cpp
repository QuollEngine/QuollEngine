#include "liquid/core/Base.h"
#include "NameComponent.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

int NameComponent::LuaInterface::get(void *state) {
  LuaScope scope(state);
  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  if (entityDatabase.hasComponent<NameComponent>(entity)) {
    scope.set(entityDatabase.getComponent<NameComponent>(entity).name);
  } else {
    scope.set<String>("");
  }

  return 1;
}

int NameComponent::LuaInterface::set(void *state) {
  LuaScope scope(state);
  if (!scope.is<String>(1)) {
    // TODO: Show logs here
    return 0;
  }

  auto string = scope.get<String>(1);
  scope.pop(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);
  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.popLast<uint32_t>();

  entityDatabase.setComponent<NameComponent>(entity, {string});

  return 0;
};

} // namespace liquid
