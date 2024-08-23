#pragma once

#include "quoll/core/Name.h"

namespace quoll {

class EntityDatabase;

class EntityQuery {
public:
  EntityQuery(EntityDatabase &entityDatabase);

  Entity getFirstEntityByName(StringView name);

private:
  flecs::query<Name> mQuery;
};

} // namespace quoll
