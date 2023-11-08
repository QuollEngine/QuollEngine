#pragma once

#include "quoll/lua-scripting/LuaHeaders.h"

namespace quoll::lua {

/**
 * @brief Noop metatable
 *
 * Every field and function in this metatable
 * does nothing.
 */
struct NoopMetatable {
  /**
   * @brief Call function
   * @return Noop metatable
   */
  NoopMetatable call();

  /**
   * @brief Index function
   * @return Noop metatable
   */
  NoopMetatable index();

  /**
   * @brief Create metatable
   *
   * @param state Sol state
   */
  static void create(sol::state &state);
};

} // namespace quoll::lua
