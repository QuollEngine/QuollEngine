#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"
#include "quoll/scripting/EntityTable.h"

namespace quoll {

/**
 * @brief Scripting interfaces for entity spawner
 */
struct EntitySpawnerScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for entity spawner
 */
class EntitySpawnerScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create entity spawner
   *
   * @param scriptGlobals Script globals
   */
  LuaInterface(ScriptGlobals &scriptGlobals);

  /**
   * @brief Spawn empty entity
   *
   * @return Newly created empty entity
   */
  EntityTable spawnEmpty();

  /**
   * @brief Spawn prefab
   *
   * @param prefab Prefab asset
   * @return Root prefab entity
   */
  sol_maybe<EntityTable> spawnPrefab(PrefabAssetHandle prefab);

  /**
   * @brief Spawn sprite
   *
   * @param texture Texture asset
   * @return Sprite entity
   */
  sol_maybe<EntityTable> spawnSprite(TextureAssetHandle texture);

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
