#pragma once

namespace quoll {

/**
 * @brief Sparse set pool for entity storage
 */
struct EntityStorageSparseSetComponentPool {
  /**
   * List of entity indices
   */
  std::vector<usize> entityIndices;

  /**
   * List of Entities
   */
  std::vector<Entity> entities;

  /**
   * List of components
   */
  std::vector<std::any> components;
};

} // namespace quoll
