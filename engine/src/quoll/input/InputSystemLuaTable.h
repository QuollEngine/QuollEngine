#pragma once

#include "quoll/lua-scripting/ScriptGlobals.h"
#include "quoll/signals/SignalLuaTable.h"

namespace quoll {

class InputSystemLuaTable {
public:
  InputSystemLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  SignalLuaTable onKeyPress();

  SignalLuaTable onKeyRelease();

  static InputSystemLuaTable create(sol::state_view state, Entity entity,
                                    ScriptGlobals scriptGlobals);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
