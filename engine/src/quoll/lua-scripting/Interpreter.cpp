#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "Interpreter.h"
#include "LuaHeaders.h"

namespace quoll::lua {

inline void errorHandler(sol::optional<std::string> message) {
  if (message) {
    Engine::getUserLogger().error() << message.value();
  }
}

lua_State *Interpreter::createState() {
  auto *state = luaL_newstate();
  lua_atpanic(state, sol::c_call<decltype(&errorHandler), &errorHandler>);
  luaL_openlibs(state);
  return state;
}

void Interpreter::destroyState(lua_State *state) { lua_close(state); }

bool Interpreter::evaluate(const std::vector<u8> &bytes, lua_State *state) {
  auto ret =
      luaL_loadstring(state, quoll::String{bytes.begin(), bytes.end()}.c_str());

  if (ret != LUA_OK) {
    return false;
  }

  ret = lua_pcall(state, 0, 0, 0);
  if (ret != LUA_OK) {
    Engine::getUserLogger().error() << lua_tostring(state, -1);
    return false;
  }

  lua_pop(state, lua_gettop(state));
  return true;
}

} // namespace quoll::lua
