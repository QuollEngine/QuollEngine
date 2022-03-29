#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

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

  auto ret = luaL_loadstring(
      luaState, liquid::String{bytes.begin(), bytes.end()}.c_str());
  LIQUID_ASSERT(ret == LUA_OK, "Cannot load script");

  ret = lua_pcall(luaState, 0, 0, 0);
  LIQUID_ASSERT(ret == LUA_OK, "Cannot evaluate script");

  lua_pop(luaState, lua_gettop(luaState));
}

void LuaInterpreter::getFunction(void *scope, const char *name) {
  auto *luaState = static_cast<lua_State *>(scope);
  lua_getglobal(luaState, name);
}

void LuaInterpreter::callFunction(void *scope, uint32_t numArgs) {
  auto *luaState = static_cast<lua_State *>(scope);

  if (lua_pcall(luaState, numArgs, 0, 0) == LUA_OK) {
    lua_pop(luaState, lua_gettop(luaState));
  } else {
    LOG_DEBUG("[Lua] Variable is not a function");
    lua_pop(luaState, -1);
  }
}

bool LuaInterpreter::hasFunction(void *scope, const char *name) {
  auto *luaState = static_cast<lua_State *>(scope);
  bool ret = lua_getglobal(luaState, name) && lua_isfunction(luaState, -1);

  lua_pop(luaState, -1);
  return ret;
}

} // namespace liquid
