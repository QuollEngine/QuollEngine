#pragma once

#include "quoll/lua-scripting/LuaHeaders.h"

namespace quoll {

class MathLuaTable {
public:
  static void create(sol::state_view state);
};

} // namespace quoll
