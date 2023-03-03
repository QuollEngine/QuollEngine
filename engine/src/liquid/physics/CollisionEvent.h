#pragma once

#include "liquid/entity/Entity.h"

namespace liquid {

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

} // namespace liquid
