#pragma once

#include "quoll/entity/EntityLuaTable.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class NameLuaTable {
public:
  static String get(EntityLuaTable &entityTable);

  static void set(EntityLuaTable &entityTable, String name);

  static void create(sol::usertype<EntityLuaTable> entityUsertype,
                     sol::state_view state);
};

} // namespace quoll
