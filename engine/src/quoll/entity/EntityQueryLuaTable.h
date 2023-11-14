#pragma once

#include "EntityLuaTable.h"

namespace quoll {

/**
 * @brief Lua scripting interface for entity query
 */
class EntityQueryLuaTable {
public:
  /**
   * @brief Create entity query
   *
   * @param scriptGlobals Script globals
   */
  EntityQueryLuaTable(ScriptGlobals scriptGlobals);

  /**
   * @brief Get first entity by name
   *
   * @param name Entity name
   * @return Found entity
   */
  sol_maybe<EntityLuaTable> getFirstEntityByName(String name);

  /**
   * @brief Delete entity
   *
   * @param entity Entity table
   */
  void deleteEntity(EntityLuaTable entity);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "entity_query"; }

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
