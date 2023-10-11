#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "LuaHeaders.h"
#include "LuaInterpreter.h"

namespace quoll {

inline void errorHandler(sol::optional<std::string> message) {
  if (message) {
    Engine::getUserLogger().error() << message.value();
  }
}

lua_State *LuaInterpreter::createState() {
  auto *state = luaL_newstate();
  lua_atpanic(state, sol::c_call<decltype(&errorHandler), &errorHandler>);
  luaL_openlibs(state);
  return state;
}

void LuaInterpreter::destroyState(lua_State *state) { lua_close(state); }

bool LuaInterpreter::evaluate(const std::vector<uint8_t> &bytes,
                              lua_State *state) {

  auto ret =
      luaL_loadstring(state, quoll::String{bytes.begin(), bytes.end()}.c_str());

  if (ret != LUA_OK) {
    return false;
  }

  ret = lua_pcall(state, 0, 0, 0);
  if (ret != LUA_OK) {
    return false;
  }

  lua_pop(state, lua_gettop(state));
  return true;
}

} // namespace quoll
