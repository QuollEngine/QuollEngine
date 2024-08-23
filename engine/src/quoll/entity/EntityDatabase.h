#pragma once

#include "Entity.h"
#include "Flecs.h"

namespace quoll {

class EntityDatabase : public flecs::world {
public:
  EntityDatabase();
};

} // namespace quoll
