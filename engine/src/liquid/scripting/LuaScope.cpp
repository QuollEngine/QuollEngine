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

void LuaScope::pop(int count) { lua_pop(mScope, count); };

bool LuaScope::luaGetBoolean(int index) {
  return static_cast<bool>(lua_toboolean(mScope, index));
}

int32_t LuaScope::luaGetInteger(int index) {
  return static_cast<int32_t>(lua_tointeger(mScope, index));
}

float LuaScope::luaGetNumber(int index) {
  return static_cast<float>(lua_tonumber(mScope, index));
}

StringView LuaScope::luaGetString(int index) {
  return StringView(lua_tostring(mScope, index));
}

void *LuaScope::luaGetUserData(int index) {
  return lua_touserdata(mScope, static_cast<int>(index));
}

bool LuaScope::luaIsNumber(int index) { return lua_isnumber(mScope, index); }

bool LuaScope::luaIsString(int index) { return lua_isstring(mScope, index); }

void LuaScope::luaSetBoolean(bool value) {
  lua_pushboolean(mScope, static_cast<int>(value));
}

void LuaScope::luaSetNumber(float value) {
  lua_pushnumber(mScope, static_cast<lua_Number>(value));
}

void LuaScope::luaSetString(const String &value) {
  lua_pushstring(mScope, value.c_str());
}

void LuaScope::luaSetUserData(void *data) {
  lua_pushlightuserdata(mScope, data);
}

int LuaScope::luaGetGlobal(const String &name) {
  return lua_getglobal(mScope, name.c_str());
}

void LuaScope::setPreviousValueAsGlobal(const String &name) {
  lua_setglobal(mScope, name.c_str());
}

bool LuaScope::hasFunction(const String &name) {
  bool ret = luaGetGlobal(name) && lua_isfunction(mScope, -1);

  pop(-1);
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

} // namespace liquid
