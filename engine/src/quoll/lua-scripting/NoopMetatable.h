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
  NoopMetatable call();

  NoopMetatable index();

  static void create(sol::state &state);
};

} // namespace quoll::lua
