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

bool LuaInterpreter::evaluate(const std::vector<char> &bytes, LuaScope &scope) {
  auto *luaState = scope.getLuaState();

  auto ret = luaL_loadstring(
      luaState, liquid::String{bytes.begin(), bytes.end()}.c_str());

  if (!ret == LUA_OK) {
    auto stringView = scope.get<StringView>(-1);
    Engine::getUserLogger().error() << "Cannot load lua script: " << stringView;
    return false;
  }

  ret = lua_pcall(luaState, 0, 0, 0);
  if (!ret == LUA_OK) {
    auto stringView = scope.get<StringView>(-1);
    Engine::getUserLogger().error()
        << "Cannot evaluate lua script: " << stringView;
    return false;
  }

  lua_pop(luaState, lua_gettop(luaState));
  return true;
}

} // namespace liquid
