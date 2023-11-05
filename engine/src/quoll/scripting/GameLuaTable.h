#pragma once

#include "LuaHeaders.h"
#include "ScriptSignal.h"
#include "ScriptSignalView.h"
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
  GameLuaTable(Entity entity, ScriptGlobals &scriptGlobals);

  /**
   * @brief Get on update signal
   *
   * @return On update signal
   */
  ScriptSignalView onUpdate();

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
