#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "UIComponents.h"

namespace quoll {

class UICanvasLuaTable {
public:
  UICanvasLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  void render(UIView view);

  static void create(sol::usertype<UICanvasLuaTable> usertype,
                     sol::state_view state);

  static const String getName() { return "uiCanvas"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
