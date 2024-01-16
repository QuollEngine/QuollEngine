#include "quoll/core/Base.h"
#include "LuaScript.h"
#include "ScriptLuaTable.h"

namespace quoll {

ScriptLuaTable::ScriptLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol::object ScriptLuaTable::get(const String &name) {
  if (!mScriptGlobals.entityDatabase.has<LuaScript>(mEntity)) {
    return sol::nil;
  }

  auto &script = mScriptGlobals.entityDatabase.get<LuaScript>(mEntity);

  script.loader.wait();

  sol::state_view state(script.state);

  if (state[name].valid()) {
    return state[name];
  }

  return sol::nil;
}

void ScriptLuaTable::set(const String &name, sol::object value) {
  if (!mScriptGlobals.entityDatabase.has<LuaScript>(mEntity)) {
    return;
  }

  auto &script = mScriptGlobals.entityDatabase.get<LuaScript>(mEntity);
  script.loader.wait();

  sol::state_view state(script.state);
  if (state[name].valid()) {
    state[name] = value;
  }
}

void ScriptLuaTable::create(sol::usertype<ScriptLuaTable> usertype,
                            sol::state_view state) {
  usertype["get"] = &ScriptLuaTable::get;
  usertype["set"] = &ScriptLuaTable::set;
}

} // namespace quoll
