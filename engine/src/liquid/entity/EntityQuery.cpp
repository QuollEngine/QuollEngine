#include "liquid/core/Base.h"

#include "EntityDatabase.h"
#include "EntityQuery.h"

namespace liquid {

EntityQuery::EntityQuery(EntityDatabase &entityDatabase)
    : mEntityDatabase(entityDatabase) {}

Entity EntityQuery::getFirstEntityByName(StringView name) {
  for (auto [entity, component] : mEntityDatabase.view<Name>()) {
    if (component.name == name) {
      return entity;
    }
  }

  return EntityNull;
}

} // namespace liquid
