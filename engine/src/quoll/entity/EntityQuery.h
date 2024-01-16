#pragma once

#include "EntityDatabase.h"

namespace quoll {

class EntityQuery {
public:
  EntityQuery(EntityDatabase &entityDatabase);

  Entity getFirstEntityByName(StringView name);

private:
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll
