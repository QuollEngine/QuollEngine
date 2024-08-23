#include "quoll/core/Base.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/window/KeyboardEvent.h"
#include "quoll/window/WindowSignals.h"
#include "InputSystemLuaTable.h"

namespace quoll {

InputSystemLuaTable::InputSystemLuaTable(Entity entity,
                                         ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

InputSystemLuaTable InputSystemLuaTable::create(sol::state_view state,
                                                Entity entity,
                                                ScriptGlobals scriptGlobals) {

  auto keyboardEvent = state.new_usertype<KeyboardEvent>(sol::no_constructor);
  keyboardEvent["key"] = &KeyboardEvent::key;
  keyboardEvent["mods"] = &KeyboardEvent::mods;

  auto usertype = state.new_usertype<InputSystemLuaTable>(sol::no_constructor);
  usertype["onKeyPress"] = sol::property(&InputSystemLuaTable::onKeyPress);
  usertype["onKeyRelease"] = sol::property(&InputSystemLuaTable::onKeyRelease);

  return InputSystemLuaTable(entity, scriptGlobals);
}

SignalLuaTable InputSystemLuaTable::onKeyPress() {
  return SignalLuaTable(mScriptGlobals.windowSignals.onKeyPress(), mEntity);
}

SignalLuaTable InputSystemLuaTable::onKeyRelease() {
  return SignalLuaTable(mScriptGlobals.windowSignals.onKeyRelease(), mEntity);
}

} // namespace quoll
