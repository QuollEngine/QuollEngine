#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "quoll/entity/EntityLuaTable.h"

namespace quoll {

/**
 * @brief Lua interface for name component
 */
class NameLuaTable {
public:
  /**
   * @brief Get name
   *
   * @param entityTable Entity table
   * @return Name
   */
  static String get(EntityLuaTable &entityTable);

  /**
   * @brief Set name
   *
   * @param entityTable Entity table
   * @param name Name
   */
  static void set(EntityLuaTable &entityTable, String name);

  /**
   * @brief Create user type
   *
   * @param entityUsertype Entity user type
   * @param state Sol state
   */
  static void create(sol::usertype<EntityLuaTable> entityUsertype,
                     sol::state_view state);
};

} // namespace quoll
