#pragma once

#include "quoll/signals/SignalLuaTable.h"
#include "LuaHeaders.h"
#include "ScriptGlobals.h"

namespace quoll {

class GameLuaTable {
public:
  GameLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  sol::object getService(String name);

  SignalLuaTable onUpdate();

  static void create(sol::state_view state);

private:
  ScriptGlobals mScriptGlobals;
  Entity mEntity;
};

} // namespace quoll
