#pragma once

namespace liquid {

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
  Entity parent = EntityNull;
};

} // namespace liquid
