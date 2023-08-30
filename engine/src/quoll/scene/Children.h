#pragma once

namespace quoll {

/**
 * @brief Children component
 *
 * Provides children relationship
 * for the entity
 */
struct Children {
  /**
   * Children entities
   */
  std::vector<Entity> children;
};

} // namespace quoll
