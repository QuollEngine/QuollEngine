#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Name.h"
#include "quoll/entity/EntityDatabase.h"
#include "NameLuaTable.h"

namespace quoll {

String NameLuaTable::get(EntityLuaTable &entityTable) {
  auto &scriptGlobals = entityTable.getScriptGlobals();
  auto entity = entityTable.getEntity();

  if (entity.has<Name>()) {
    return entity.get_ref<Name>()->name;
  }

  return "";
}

void NameLuaTable::set(EntityLuaTable &entityTable, String name) {
  auto &scriptGlobals = entityTable.getScriptGlobals();
  auto entity = entityTable.getEntity();

  entity.set<Name>({name});
}

void NameLuaTable::create(sol::usertype<EntityLuaTable> entityUsertype,
                          sol::state_view state) {
  entityUsertype["name"] = sol::property(get, set);
}

} // namespace quoll
