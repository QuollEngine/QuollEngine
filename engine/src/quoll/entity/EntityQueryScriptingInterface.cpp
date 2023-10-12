#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/scripting/ScriptDecorator.h"
#include "quoll/scripting/LuaMessages.h"

#include "EntityQuery.h"
#include "EntityQueryScriptingInterface.h"

namespace quoll {

EntityQueryScriptingInterface::LuaInterface::LuaInterface(
    ScriptGlobals &scriptGlobals)
    : mScriptGlobals(scriptGlobals) {}

sol_maybe<EntityTable>
EntityQueryScriptingInterface::LuaInterface::getFirstEntityByName(String name) {
  EntityQuery query(mScriptGlobals.entityDatabase);

  auto entity = query.getFirstEntityByName(name);
  if (entity == Entity::Null) {
    return sol::nil;
  }

  return EntityTable(entity, mScriptGlobals);
}

void EntityQueryScriptingInterface::LuaInterface::deleteEntity(
    EntityTable entity) {
  if (!mScriptGlobals.entityDatabase.exists(entity.getEntity())) {
    Engine::getUserLogger().error() << LuaMessages::entityDoesNotExist(
        getName(), "delete_entity", entity.getEntity());
    return;
  }

  mScriptGlobals.entityDatabase.set<Delete>(entity.getEntity(), {});
}

void EntityQueryScriptingInterface::LuaInterface::create(
    sol::state_view state) {
  auto usertype =
      state.new_usertype<EntityQueryScriptingInterface::LuaInterface>(
          "EntityQuery");

  usertype["get_first_entity_by_name"] = &LuaInterface::getFirstEntityByName;
  usertype["delete_entity"] = &LuaInterface::deleteEntity;
}

} // namespace quoll
