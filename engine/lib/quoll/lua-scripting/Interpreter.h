#pragma once

#include "LuaHeaders.h"

namespace quoll::lua {

class Interpreter {
public:
  lua_State *createState();

  void destroyState(lua_State *state);

  bool evaluate(const std::vector<u8> &bytes, lua_State *state);
};

} // namespace quoll::lua
