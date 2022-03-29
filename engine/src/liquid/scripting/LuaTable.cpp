#include "liquid/core/Base.h"
#include "LuaTable.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace liquid {

LuaTable::LuaTable(void *scope, uint32_t size) : mScope(scope), mSize(size) {
  lua_createtable(static_cast<lua_State *>(mScope), 0, static_cast<int>(mSize));
}

void LuaTable::set(const char *label, uint32_t value) {
  lua_pushinteger(static_cast<lua_State *>(mScope),
                  static_cast<lua_Integer>(value));
  lua_setfield(static_cast<lua_State *>(mScope), -2, label);
}

} // namespace liquid
