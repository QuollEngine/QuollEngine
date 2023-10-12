#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"
#include "quoll/scripting/EntityTable.h"

namespace quoll {

/**
 * @brief Scripting interfaces for entity query
 */
struct EntityQueryScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for entity query
 */
class EntityQueryScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create entity query
   *
   * @param scriptGlobals Script globals
   */
  LuaInterface(ScriptGlobals &scriptGlobals);

  /**
   * @brief Get first entity by name
   *
   * @param name Entity name
   * @return Found entity
   */
  sol_maybe<EntityTable> getFirstEntityByName(String name);

  /**
   * @brief Delete entity
   *
   * @param entity Entity table
   */
  void deleteEntity(EntityTable entity);

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
