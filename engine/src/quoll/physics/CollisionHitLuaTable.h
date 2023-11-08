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
   */
  static void create(sol::state_view state);
};

} // namespace quoll
