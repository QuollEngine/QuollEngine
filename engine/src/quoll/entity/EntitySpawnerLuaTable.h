#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "EntityLuaTable.h"

namespace quoll {

/**
 * @brief Lua scripting interface for entity spawner
 */
class EntitySpawnerLuaTable {
public:
  /**
   * @brief Create entity spawner
   *
   * @param scriptGlobals Script globals
   */
  EntitySpawnerLuaTable(ScriptGlobals scriptGlobals);

  /**
   * @brief Spawn empty entity
   *
   * @return Newly created empty entity
   */
  EntityLuaTable spawnEmpty();

  /**
   * @brief Spawn prefab
   *
   * @param prefab Prefab asset
   * @return Root prefab entity
   */
  sol_maybe<EntityLuaTable> spawnPrefab(PrefabAssetHandle prefab);

  /**
   * @brief Spawn sprite
   *
   * @param texture Texture asset
   * @return Sprite entity
   */
  sol_maybe<EntityLuaTable> spawnSprite(TextureAssetHandle texture);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "entity_spawner"; }

  /**
   * @brief Create user type
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);

private:
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
