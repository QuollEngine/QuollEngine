#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/scripting/ScriptDecorator.h"
#include "liquid/scripting/LuaMessages.h"

#include "EntityQuery.h"
#include "EntityQueryScriptingInterface.h"

namespace quoll {

int EntityQueryScriptingInterface::LuaInterface::getFirstEntityByName(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<String>(1)) {
    scope.set(nullptr);
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<String>(
        getName(), "get_first_entity_by_name");

    return 1;
  }

  auto arg = scope.get<StringView>(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  EntityQuery query(entityDatabase);

  auto entity = query.getFirstEntityByName(arg);
  if (entity == Entity::Null) {
    scope.set(nullptr);
  } else {
    ScriptDecorator::createEntityTable(scope, entity);
  }

  return 1;
}

int EntityQueryScriptingInterface::LuaInterface::deleteEntity(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<Entity>(getName(), "delete_entity");
    return 0;
  }

  auto arg = scope.get<LuaTable>(1);
  arg.get("id");
  if (!scope.is<uint32_t>()) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<Entity>(getName(), "delete_entity");
    return 0;
  }

  auto entity = scope.get<Entity>();
  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.exists(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::entityDoesNotExist(getName(), "delete_entity", entity);
    return 0;
  }

  entityDatabase.set<Delete>(entity, {});

  return 0;
}

} // namespace quoll
