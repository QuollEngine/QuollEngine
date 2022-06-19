#include "liquid/core/Base.h"
#include "NameComponent.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

int NameComponent::LuaInterface::get(void *state) {
  LuaScope scope(state);
  EntityContext &entityContext = *static_cast<EntityContext *>(
      scope.getGlobal<LuaUserData>("__privateContext").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  if (entityContext.hasComponent<NameComponent>(entity)) {
    scope.set(entityContext.getComponent<NameComponent>(entity).name);
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

  EntityContext &entityContext = *static_cast<EntityContext *>(
      scope.getGlobal<LuaUserData>("__privateContext").pointer);
  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.popLast<uint32_t>();

  entityContext.setComponent<NameComponent>(entity, {string});

  return 0;
};

} // namespace liquid
