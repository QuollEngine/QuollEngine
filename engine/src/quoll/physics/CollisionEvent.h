#pragma once

#include "quoll/entity/Entity.h"

namespace quoll {

enum class CollisionEvent { CollisionStarted, CollisionEnded };

/**
 * @brief Collision event data
 */
struct CollisionObject {
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
