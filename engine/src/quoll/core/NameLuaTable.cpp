#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Name.h"

#include "quoll/entity/EntityDatabase.h"

#include "NameLuaTable.h"

namespace quoll {

String NameLuaTable::get(EntityLuaTable &entityTable) {
  auto &scriptGlobals = entityTable.getScriptGlobals();
  auto entity = entityTable.getEntity();

  if (scriptGlobals.entityDatabase.has<Name>(entity)) {
    return scriptGlobals.entityDatabase.get<Name>(entity).name;
  }

  return "";
}

void NameLuaTable::set(EntityLuaTable &entityTable, String name) {
  auto &scriptGlobals = entityTable.getScriptGlobals();
  auto entity = entityTable.getEntity();

  return scriptGlobals.entityDatabase.set<Name>(entity, {name});
}

void NameLuaTable::create(sol::usertype<EntityLuaTable> entityUsertype,
                          sol::state_view state) {
  entityUsertype["name"] = sol::property(get, set);
}

} // namespace quoll
