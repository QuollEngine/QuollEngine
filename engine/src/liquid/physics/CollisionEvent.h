#pragma once

#include "liquid/entity/Entity.h"

namespace liquid {

enum class CollisionEvent { CollisionStarted, CollisionEnded };

struct CollisionObject {
  Entity a = ENTITY_MAX, b = ENTITY_MAX;
};

} // namespace liquid
