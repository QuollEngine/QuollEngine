#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "Children.h"
#include "EntityRelationUtils.h"
#include "Parent.h"
#include "ParentLuaTable.h"

namespace quoll {

static sol_maybe<EntityLuaTable> getParent(EntityLuaTable &entityTable) {
  auto entity = entityTable.getEntity();
  auto &scriptGlobals = entityTable.getScriptGlobals();
  if (scriptGlobals.entityDatabase.has<Parent>(entity)) {
    auto parent = scriptGlobals.entityDatabase.get<Parent>(entity).parent;

    return EntityLuaTable(parent, entityTable.getScriptGlobals());
  }

  return sol::nil;
}

static void setParent(EntityLuaTable &entityTable,
                      sol_maybe<EntityLuaTable> parentTable) {
  auto &scriptGlobals = entityTable.getScriptGlobals();
  auto &db = entityTable.getScriptGlobals().entityDatabase;
  auto entity = entityTable.getEntity();

  if (std::get_if<sol::nil_t>(&parentTable)) {
    EntityRelationUtils::removeEntityParent(db, entity);
  } else if (auto *newParent = std::get_if<EntityLuaTable>(&parentTable)) {
    auto status = EntityRelationUtils::setEntityParent(db, entity,
                                                       newParent->getEntity());

    if (status == EntityReparentStatus::CannotParentEntityToDescendant) {
      Engine::getUserLogger().error()
          << "Entity cannot be parented to its child";
    }
  }
}

void ParentLuaTable::create(sol::usertype<EntityLuaTable> entityUsertype,
                            sol::state_view state) {
  entityUsertype["parent"] = sol::property(getParent, setParent);
}

} // namespace quoll
