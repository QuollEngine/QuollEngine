#pragma once

#include "quoll/entity/Entity.h"

namespace quoll {

/**
 * @brief Parent component
 *
 * Stores parent for
 * the entity
 */
struct Parent {
  /**
   * Parent entity
   */
  Entity parent = Entity::Null;
};

} // namespace quoll
