#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "quoll/renderer/TextureAsset.h"
#include "quoll/scene/PrefabAsset.h"
#include "EntityLuaTable.h"

namespace quoll {

class EntitySpawnerLuaTable {
public:
  EntitySpawnerLuaTable(ScriptGlobals scriptGlobals);

  EntityLuaTable spawnEmpty();

  sol_maybe<EntityLuaTable> spawnPrefab(AssetHandleType prefab);

  sol_maybe<EntityLuaTable> spawnSprite(AssetHandleType texture);

  static void create(sol::state_view state);

private:
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
