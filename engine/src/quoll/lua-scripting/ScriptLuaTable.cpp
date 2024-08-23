#include "quoll/core/Base.h"
#include "quoll/entity/EntityDatabase.h"
#include "LuaScript.h"
#include "ScriptLuaTable.h"

namespace quoll {

ScriptLuaTable::ScriptLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol::object ScriptLuaTable::get(const String &name) {
  if (!mEntity.has<LuaScript>()) {
    return sol::nil;
  }

  auto script = mEntity.get_ref<LuaScript>();

  script->loader.wait();

  sol::state_view state(script->state);

  if (state[name].valid()) {
    return state[name];
  }

  return sol::nil;
}

void ScriptLuaTable::set(const String &name, sol::object value) {
  if (!mEntity.has<LuaScript>()) {
    return;
  }

  auto script = mEntity.get_ref<LuaScript>();
  script->loader.wait();

  sol::state_view state(script->state);
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
