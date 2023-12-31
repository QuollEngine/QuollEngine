#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Name.h"

#include "quoll/entity/EntityDatabase.h"

#include "NameLuaTable.h"

namespace quoll {

NameLuaTable::NameLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mScriptGlobals(scriptGlobals), mEntity(entity) {}

String NameLuaTable::get() {
  if (mScriptGlobals.entityDatabase.has<Name>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Name>(mEntity).name;
  }

  return "";
}

void NameLuaTable::set(String name) {
  mScriptGlobals.entityDatabase.set<Name>(mEntity, {name});
}

void NameLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Name>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Name>(mEntity);
  }
}

void NameLuaTable::create(sol::usertype<NameLuaTable> usertype,
                          sol::state_view state) {
  usertype["value"] = sol::property(&NameLuaTable::get, &NameLuaTable::set);
  usertype["delete"] = &NameLuaTable::deleteThis;
}

} // namespace quoll
