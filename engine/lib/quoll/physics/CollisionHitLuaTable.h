#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class CollisionHitLuaTable {
public:
  static void create(sol::state_view state, ScriptGlobals scriptGlobals);
};

} // namespace quoll
