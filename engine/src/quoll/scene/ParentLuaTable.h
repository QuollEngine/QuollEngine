#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "quoll/entity/EntityLuaTable.h"

namespace quoll {

/**
 * @brief Parent lua table
 */
class ParentLuaTable {
public:
  /**
   * @brief Create parent lua table
   *
   * @param entityUsertype Entity usertype
   * @param state Sol state
   */
  static void create(sol::usertype<EntityLuaTable> entityUsertype,
                     sol::state_view state);
};

} // namespace quoll
