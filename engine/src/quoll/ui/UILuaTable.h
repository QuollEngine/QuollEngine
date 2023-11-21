#pragma once

#include "quoll/lua-scripting/LuaHeaders.h"
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
   * @return UI Lua table
   */
  static sol::table create(sol::state_view state);
};

} // namespace quoll
