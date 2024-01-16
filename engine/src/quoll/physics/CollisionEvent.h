#pragma once

#include "quoll/entity/Entity.h"

namespace quoll {

struct CollisionEvent {
  Entity a = Entity::Null;

  Entity b = Entity::Null;
};

} // namespace quoll
