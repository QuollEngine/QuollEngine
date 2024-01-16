#pragma once

#include "LuaHeaders.h"
#include "LuaScriptInputVariable.h"

namespace quoll::lua {

class InputVars {
public:
  InputVars(std::unordered_map<String, LuaScriptInputVariable> &variables);

  std::variant<String, u32> registerVar(String name,
                                        LuaScriptVariableType type);

  static void create(sol::state_view state);

private:
  std::unordered_map<String, LuaScriptInputVariable> mVariables;
};

} // namespace quoll::lua
