#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "AnimationStateLuaTable.h"

namespace quoll {

class AnimatorLuaTable {
public:
  AnimatorLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  sol_maybe<f32> getNormalizedTime();

  sol_maybe<AnimationStateLuaTable> getCurrentState();

  void trigger(String event);

  void deleteThis();

  static const String getName() { return "animator"; }

  static void create(sol::usertype<AnimatorLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
