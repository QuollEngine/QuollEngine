#pragma once

#include "quoll/entity/Entity.h"

namespace quoll {

/**
 * @brief Collision event data
 */
struct CollisionEvent {
  /**
   * Collided entity A
   */
  Entity a = Entity::Null;

  /**
   * Collided entity B
   */
  Entity b = Entity::Null;
};

} // namespace quoll
