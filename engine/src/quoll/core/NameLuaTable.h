#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "quoll/entity/EntityLuaTable.h"

namespace quoll {

class NameLuaTable {
public:
  static String get(EntityLuaTable &entityTable);

  static void set(EntityLuaTable &entityTable, String name);

  static void create(sol::usertype<EntityLuaTable> entityUsertype,
                     sol::state_view state);
};

} // namespace quoll
