#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "LuaInterpreter.h"
#include "LuaScope.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace liquid {

LuaScope LuaInterpreter::createScope() {
  auto *state = luaL_newstate();
  luaL_openlibs(state);
  return LuaScope(state);
}

void LuaInterpreter::destroyScope(LuaScope &scope) {
  lua_close(scope.getLuaState());
}

void LuaInterpreter::evaluate(const std::vector<char> &bytes, LuaScope &scope) {
  auto *luaState = scope.getLuaState();

  auto ret = luaL_loadstring(
      luaState, liquid::String{bytes.begin(), bytes.end()}.c_str());
  LIQUID_ASSERT(ret == LUA_OK, "Cannot load script");

  ret = lua_pcall(luaState, 0, 0, 0);
  LIQUID_ASSERT(ret == LUA_OK, "Cannot evaluate script");

  lua_pop(luaState, lua_gettop(luaState));
}

} // namespace liquid
