#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

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
