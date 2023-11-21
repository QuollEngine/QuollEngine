#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief User logger Lua table
 */
class UserLoggerLuaTable {
public:
  /**
   * @brief Create user logger user type
   *
   * @param state Sol state
   * @return User logger Lua table
   */
  static sol::table create(sol::state_view state);
};

} // namespace quoll
