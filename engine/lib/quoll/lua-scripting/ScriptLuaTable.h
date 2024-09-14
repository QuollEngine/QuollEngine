#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class ScriptLuaTable {
public:
  ScriptLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  sol::object get(const String &name);

  void set(const String &name, sol::object value);

  static const String getName() { return "script"; }

  static void create(sol::usertype<ScriptLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
