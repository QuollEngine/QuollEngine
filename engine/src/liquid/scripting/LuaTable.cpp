#include "liquid/core/Base.h"
#include "LuaTable.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace liquid {

LuaTable::LuaTable(void *scope) : mScope(scope) {}

void LuaTable::set(const char *key, uint32_t value) {
  lua_pushinteger(static_cast<lua_State *>(mScope),
                  static_cast<lua_Integer>(value));
  lua_setfield(static_cast<lua_State *>(mScope), -2, key);
}

void LuaTable::set(const char *key, int (*value)(void *)) {
  lua_pushcfunction(static_cast<lua_State *>(mScope),
                    reinterpret_cast<lua_CFunction>(value));
  lua_setfield(static_cast<lua_State *>(mScope), -2, key);
}

void LuaTable::set(const char *key, const LuaTable &value) {
  lua_setfield(static_cast<lua_State *>(mScope), -2, key);
}

void LuaTable::get(const char *key) {
  lua_getfield(static_cast<lua_State *>(mScope), -1, key);
}

} // namespace liquid
