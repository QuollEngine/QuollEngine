#pragma once

#include "quoll/entity/EntityLuaTable.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class ParentLuaTable {
public:
  static void create(sol::usertype<EntityLuaTable> entityUsertype,
                     sol::state_view state);
};

} // namespace quoll
