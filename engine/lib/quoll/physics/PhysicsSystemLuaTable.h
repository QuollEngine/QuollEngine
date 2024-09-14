#pragma once

#include "quoll/lua-scripting/ScriptGlobals.h"
#include "quoll/signals/SignalLuaTable.h"

namespace quoll {

class PhysicsSystemLuaTable {
public:
  PhysicsSystemLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  SignalLuaTable onCollisionStart();

  SignalLuaTable onCollisionEnd();

  static PhysicsSystemLuaTable create(sol::state_view state, Entity entity,
                                      ScriptGlobals scriptGlobals);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
