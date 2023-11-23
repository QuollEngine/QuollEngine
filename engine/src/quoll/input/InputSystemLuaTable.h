#pragma once

#include "quoll/lua-scripting/ScriptGlobals.h"
#include "quoll/lua-scripting/ScriptSignal.h"
#include "quoll/lua-scripting/ScriptSignalView.h"

namespace quoll {

/**
 * @brief Input system lua table
 */
class InputSystemLuaTable {
public:
  /**
   * @brief Input system lua table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  InputSystemLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get on key down signal
   *
   * @return On key down signal
   */
  lua::ScriptSignalView onKeyPress();

  /**
   * @brief Get on key up signal
   *
   * @return On key up signal
   */
  lua::ScriptSignalView onKeyRelease();

  /**
   * @brief Create input system lua table
   *
   * @param state Sol state
   * @param entity Entity
   * @param scriptGlobals Script globals
   * @return Input system lua table
   */
  static InputSystemLuaTable create(sol::state_view state, Entity entity,
                                    ScriptGlobals scriptGlobals);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
