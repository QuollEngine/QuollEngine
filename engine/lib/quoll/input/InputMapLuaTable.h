#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class InputMapLuaTable {
public:
  InputMapLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  sol_maybe<usize> getCommand(String name);

  sol_maybe<bool> getCommandValueBoolean(usize command);

  std::tuple<sol_maybe<f32>, sol_maybe<f32>>
  getCommandValueAxis2d(usize command);

  void setScheme(String name);

  static const String getName() { return "input"; }

  static void create(sol::usertype<InputMapLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
