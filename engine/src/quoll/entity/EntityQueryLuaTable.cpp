#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "quoll/lua-scripting/ScriptDecorator.h"
#include "EntityQuery.h"
#include "EntityQueryLuaTable.h"

namespace quoll {

EntityQueryLuaTable::EntityQueryLuaTable(ScriptGlobals scriptGlobals)
    : mScriptGlobals(scriptGlobals) {}

sol_maybe<EntityLuaTable>
EntityQueryLuaTable::getFirstEntityByName(String name) {
  EntityQuery query(mScriptGlobals.entityDatabase);

  auto entity = query.getFirstEntityByName(name);
  if (!entity) {
    return sol::nil;
  }

  return EntityLuaTable(entity, mScriptGlobals);
}

void EntityQueryLuaTable::deleteEntity(EntityLuaTable entity) {
  if (!entity.getEntity().is_valid()) {
    Engine::getUserLogger().error() << lua::Messages::entityDoesNotExist(
        "EntityQuery", "deleteEntity", entity.getEntity());
    return;
  }

  entity.getEntity().add<Delete>();
}

void EntityQueryLuaTable::create(sol::state_view state) {
  auto usertype = state.new_usertype<EntityQueryLuaTable>("EntityQuery");

  usertype["getFirstEntityByName"] = &EntityQueryLuaTable::getFirstEntityByName;
  usertype["deleteEntity"] = &EntityQueryLuaTable::deleteEntity;
}

} // namespace quoll
