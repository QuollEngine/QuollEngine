#include "liquid/core/Base.h"

#include "EntityDatabase.h"
#include "EntityQuery.h"

namespace liquid {

EntityQuery::EntityQuery(EntityDatabase &entityDatabase)
    : mEntityDatabase(entityDatabase) {}

Entity EntityQuery::getFirstEntityByName(StringView name) {
  Entity found = EntityNull;
  mEntityDatabase.iterateEntities<Name>(
      [&found, &name](auto entity, const auto &component) {
        if (found != EntityNull)
          return;

        if (component.name == name) {
          found = entity;
        }
      });

  return found;
}

} // namespace liquid
