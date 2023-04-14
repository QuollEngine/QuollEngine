#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "LuaHeaders.h"
#include "LuaInterpreter.h"
#include "LuaScope.h"

namespace liquid {

LuaScope LuaInterpreter::createScope() {
  auto *state = luaL_newstate();
  luaL_openlibs(state);
  return LuaScope(state);
}

void LuaInterpreter::destroyScope(LuaScope &scope) {
  lua_close(scope.getLuaState());
}

bool LuaInterpreter::evaluate(const std::vector<uint8_t> &bytes,
                              LuaScope &scope) {
  auto *luaState = scope.getLuaState();

  auto ret = luaL_loadstring(
      luaState, liquid::String{bytes.begin(), bytes.end()}.c_str());

  if (!ret == LUA_OK) {
    return false;
  }

  ret = lua_pcall(luaState, 0, 0, 0);
  if (!ret == LUA_OK) {
    return false;
  }

  lua_pop(luaState, lua_gettop(luaState));
  return true;
}

} // namespace liquid
