#pragma once

#include "EntityLuaTable.h"

namespace quoll {

class EntityQueryLuaTable {
public:
  EntityQueryLuaTable(ScriptGlobals scriptGlobals);

  sol_maybe<EntityLuaTable> getFirstEntityByName(String name);

  void deleteEntity(EntityLuaTable entity);

  static void create(sol::state_view state);

private:
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
