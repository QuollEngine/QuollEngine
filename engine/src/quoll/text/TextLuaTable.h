#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class TextLuaTable {
public:
  TextLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  sol_maybe<String> getContent();

  void setContent(String content);

  sol_maybe<f32> getLineHeight();

  void setLineHeight(f32 lineHeight);

  void deleteThis();

  static const String getName() { return "text"; }

  static void create(sol::usertype<TextLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
