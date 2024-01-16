#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "EntityLuaTable.h"

namespace quoll {

class EntitySpawnerLuaTable {
public:
  EntitySpawnerLuaTable(ScriptGlobals scriptGlobals);

  EntityLuaTable spawnEmpty();

  sol_maybe<EntityLuaTable> spawnPrefab(PrefabAssetHandle prefab);

  sol_maybe<EntityLuaTable> spawnSprite(TextureAssetHandle texture);

  static void create(sol::state_view state);

private:
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
