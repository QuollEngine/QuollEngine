#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "EntityDatabase.h"
#include "EntityQuery.h"

namespace quoll {

EntityQuery::EntityQuery(EntityDatabase &entityDatabase)
    : mEntityDatabase(entityDatabase) {}

Entity EntityQuery::getFirstEntityByName(StringView name) {
  for (auto [entity, component] : mEntityDatabase.view<Name>()) {
    if (component.name == name) {
      return entity;
    }
  }

  return Entity::Null;
}

} // namespace quoll
