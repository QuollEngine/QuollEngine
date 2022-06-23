#pragma once

#include "liquid/entity/EntityDatabase.h"

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
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);
};

} // namespace liquid
