#pragma once

#include "EntityDatabase.h"

namespace quoll {

/**
 * @brief Entity query
 */
class EntityQuery {
public:
  /**
   * @brief Create entity query
   *
   * @param entityDatabase Entity database
   */
  EntityQuery(EntityDatabase &entityDatabase);

  /**
   * @brief Get first found entity by name
   *
   * @param name Entity name
   * @return Entity
   */
  Entity getFirstEntityByName(StringView name);

private:
  EntityDatabase &mEntityDatabase;
};

} // namespace quoll
