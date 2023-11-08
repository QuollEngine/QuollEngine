#pragma once

#include "quoll/lua-scripting/LuaHeaders.h"

namespace quoll {

/**
 * @brief Math interfaces
 */
class MathLuaTable {
public:
  /**
   * @brief Create interfaces
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);
};

} // namespace quoll
