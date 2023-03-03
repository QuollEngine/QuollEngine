#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/scripting/LuaMessages.h"
#include "liquid/entity/EntityDatabase.h"

#include "NameScriptingInterface.h"

namespace liquid {

int NameScriptingInterface::LuaInterface::get(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get");
    scope.set<String>("");
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (entityDatabase.has<Name>(entity)) {
    scope.set(entityDatabase.get<Name>(entity).name);
  } else {
    scope.set<String>("");
  }

  return 1;
}

int NameScriptingInterface::LuaInterface::set(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set");
    return 0;
  }

  if (!scope.is<String>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<String>(getName(), "set");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto string = scope.get<String>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.set<Name>(entity, {string});

  return 0;
};

} // namespace liquid
