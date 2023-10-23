#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Delete.h"

#include "quoll/scripting/ScriptDecorator.h"
#include "quoll/scripting/LuaMessages.h"

#include "EntityQuery.h"
#include "EntityQueryLuaTable.h"

namespace quoll {

EntityQueryLuaTable::EntityQueryLuaTable(ScriptGlobals &scriptGlobals)
    : mScriptGlobals(scriptGlobals) {}

sol_maybe<EntityTable> EntityQueryLuaTable::getFirstEntityByName(String name) {
  EntityQuery query(mScriptGlobals.entityDatabase);

  auto entity = query.getFirstEntityByName(name);
  if (entity == Entity::Null) {
    return sol::nil;
  }

  return EntityTable(entity, mScriptGlobals);
}

void EntityQueryLuaTable::deleteEntity(EntityTable entity) {
  if (!mScriptGlobals.entityDatabase.exists(entity.getEntity())) {
    Engine::getUserLogger().error() << LuaMessages::entityDoesNotExist(
        getName(), "delete_entity", entity.getEntity());
    return;
  }

  mScriptGlobals.entityDatabase.set<Delete>(entity.getEntity(), {});
}

void EntityQueryLuaTable::create(sol::state_view state) {
  auto usertype = state.new_usertype<EntityQueryLuaTable>("EntityQuery");

  usertype["get_first_entity_by_name"] =
      &EntityQueryLuaTable::getFirstEntityByName;
  usertype["delete_entity"] = &EntityQueryLuaTable::deleteEntity;
}

} // namespace quoll
