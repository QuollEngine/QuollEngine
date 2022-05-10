#pragma once

namespace liquid {

/**
 * @brief Children component
 *
 * Provides children relationship
 * for the entity
 */
struct ChildrenComponent {
  /**
   * Children entities
   */
  std::vector<Entity> children;
};

} // namespace liquid
