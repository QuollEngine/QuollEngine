#include "quoll/core/Base.h"
#include "quoll/window/KeyboardEvent.h"
#include "InputSystemLuaTable.h"

namespace quoll {

InputSystemLuaTable::InputSystemLuaTable(Entity entity,
                                         ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

InputSystemLuaTable InputSystemLuaTable::create(sol::state_view state,
                                                Entity entity,
                                                ScriptGlobals scriptGlobals) {

  auto keyboardEvent =
      state.new_usertype<KeyboardEventObject>(sol::no_constructor);
  keyboardEvent["key"] = &KeyboardEventObject::key;
  keyboardEvent["mods"] = &KeyboardEventObject::mods;

  auto usertype = state.new_usertype<InputSystemLuaTable>(sol::no_constructor);
  usertype["onKeyPress"] = sol::property(&InputSystemLuaTable::onKeyPress);
  usertype["onKeyRelease"] = sol::property(&InputSystemLuaTable::onKeyRelease);

  return InputSystemLuaTable(entity, scriptGlobals);
}

SignalLuaTable InputSystemLuaTable::onKeyPress() {
  auto &script = mScriptGlobals.entityDatabase.get<LuaScript>(mEntity);
  return SignalLuaTable(mScriptGlobals.windowSignals.getKeyDownSignal(),
                        script);
}

SignalLuaTable InputSystemLuaTable::onKeyRelease() {
  auto &script = mScriptGlobals.entityDatabase.get<LuaScript>(mEntity);
  return SignalLuaTable(mScriptGlobals.windowSignals.getKeyUpSignal(), script);
}

} // namespace quoll
