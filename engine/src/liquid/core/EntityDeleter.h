#pragma once

#include "liquid/entity/EntityContext.h"

namespace liquid {

/**
 * @brief Entity deleter
 *
 * Deletes entities that are
 * flagged for deletion
 */
class EntityDeleter {
public:
  /**
   * @brief Delete entities
   *
   * @param entityContext Entity context
   */
  void update(EntityContext &entityContext);
};

} // namespace liquid
