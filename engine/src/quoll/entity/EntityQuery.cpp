#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "EntityDatabase.h"
#include "EntityQuery.h"

namespace quoll {

EntityQuery::EntityQuery(EntityDatabase &entityDatabase) {
  mQuery = entityDatabase.query<Name>();
}

Entity EntityQuery::getFirstEntityByName(StringView name) {
  return mQuery.find(
      [&name](const auto &component) { return name == component.name; });
}

} // namespace quoll
