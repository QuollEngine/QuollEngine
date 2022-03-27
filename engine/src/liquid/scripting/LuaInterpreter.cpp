#include "liquid/core/Base.h"
#include "LuaInterpreter.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace liquid {

void *LuaInterpreter::createScope() {
  auto *state = luaL_newstate();
  luaL_openlibs(state);

  return state;
}

void LuaInterpreter::destroyScope(void *scope) {
  lua_close(static_cast<lua_State *>(scope));
}

void LuaInterpreter::evaluate(const std::vector<char> &bytes, void *state) {
  auto *luaState = static_cast<lua_State *>(state);

  auto ret = luaL_loadstring(luaState, bytes.data());
  LIQUID_ASSERT(ret == LUA_OK, "Cannot load script");

  ret = lua_pcall(luaState, 0, 0, 0);
  LIQUID_ASSERT(ret == LUA_OK, "Cannot evaluate script");

  lua_pop(luaState, lua_gettop(luaState));
}

} // namespace liquid
