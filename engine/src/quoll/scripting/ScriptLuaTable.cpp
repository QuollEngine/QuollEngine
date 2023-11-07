#include "quoll/core/Base.h"
#include "ScriptLuaTable.h"

namespace quoll {

ScriptLuaTable::ScriptLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol::object ScriptLuaTable::get(const String &name) {
  if (!mScriptGlobals.entityDatabase.has<Script>(mEntity)) {
    return sol::nil;
  }

  auto &script = mScriptGlobals.entityDatabase.get<Script>(mEntity);

  script.loader.wait();

  sol::state_view state(script.state);

  if (state[name].valid()) {
    return state[name];
  }

  return sol::nil;
}

void ScriptLuaTable::create(sol::usertype<ScriptLuaTable> usertype) {
  usertype["get"] = &ScriptLuaTable::get;
}

} // namespace quoll
