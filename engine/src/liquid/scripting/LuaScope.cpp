#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "LuaScope.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace liquid {

LuaScope::LuaScope(void *scope) : mScope(static_cast<lua_State *>(scope)) {}

void LuaScope::setPreviousValueAsGlobal(const String &name) {
  lua_setglobal(mScope, name.c_str());
}

void LuaScope::luaSetUserData(void *data) {
  lua_pushlightuserdata(mScope, data);
}

bool LuaScope::hasFunction(const String &name) {
  bool ret = luaGetGlobal(name) && lua_isfunction(mScope, -1);

  luaPop(-1);
  return ret;
}

void LuaScope::call(uint32_t numArgs) {
  if (lua_pcall(mScope, numArgs, 0, 0) == LUA_OK) {
    lua_pop(mScope, lua_gettop(mScope));
  } else {
    LOG_DEBUG("[Lua] Failed to call Lua function");
    lua_pop(mScope, -1);
  }
}

LuaTable LuaScope::createTable(uint32_t size) {
  lua_createtable(mScope, 0, static_cast<int>(size));
  return LuaTable(mScope);
}

int LuaScope::luaGetGlobal(const String &name) {
  return lua_getglobal(mScope, name.c_str());
}

void *LuaScope::luaGetUserData(int index) {
  auto *value = lua_touserdata(mScope, static_cast<int>(index));
  luaPop(index);
  return value;
}

void LuaScope::luaPop(int index) { lua_pop(mScope, index); };

int32_t LuaScope::luaGetInteger(int index) {
  auto value = static_cast<int32_t>(lua_tointeger(mScope, index));
  luaPop(index);

  return value;
}

} // namespace liquid
