#pragma once

#include "quoll/scripting/LuaHeaders.h"
#include "UIComponents.h"

namespace quoll {

/**
 * @brief Lua table for UI components
 */
class UILuaTable {
public:
  /**
   * @brief Create Lua table
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);
};

} // namespace quoll
