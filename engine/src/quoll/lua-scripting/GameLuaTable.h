#pragma once

#include "quoll/signals/SignalLuaTable.h"
#include "LuaHeaders.h"
#include "ScriptGlobals.h"

namespace quoll {

/**
 * @brief Game lua table
 */
class GameLuaTable {
public:
  /**
   * @brief Create lua table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  GameLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get service
   *
   * @param name Service name
   * @return Service Lua table
   */
  sol::object get(String name);

  /**
   * @brief Get on update signal
   *
   * @return On update signal
   */
  SignalLuaTable onUpdate();

  /**
   * @brief Create game sol user type
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);

private:
  ScriptGlobals mScriptGlobals;
  Entity mEntity;
};

} // namespace quoll
