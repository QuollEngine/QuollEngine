#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Collision hit table
 */
class CollisionHitLuaTable {
public:
  /**
   * @brief Create interface
   *
   * @param state Sol state
   * @param scriptGlobals Script globals
   */
  static void create(sol::state_view state, ScriptGlobals scriptGlobals);
};

} // namespace quoll
