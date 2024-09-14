#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class UserLoggerLuaTable {
public:
  static sol::table create(sol::state_view state);
};

} // namespace quoll
