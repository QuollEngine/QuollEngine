#pragma once

namespace liquid {

/**
 * @brief Parent component
 *
 * Stores parent for
 * the entity
 */
struct ParentComponent {
  /**
   * Parent entity
   */
  Entity parent = EntityNull;
};

} // namespace liquid
