#pragma once

#include "quoll/entity/EntityDatabase.h"

namespace quoll {

enum class EntityReparentStatus {
  Ok,
  ParentHasNotChanged,
  CannotParentEntityToDescendant
};

/**
 * @brief Entity relation utilities
 */
class EntityRelationUtils {
public:
  /**
   * @brief Remove parent from entity
   *
   * @param entityDatabase Entity database
   * @param entity Entity
   */
  static void removeEntityParent(EntityDatabase &entityDatabase, Entity entity);

  /**
   * @brief Set entity parent
   *
   * @param entityDatabase Entity database
   * @param entity Entity
   * @param parent Parent
   * @return Entity parent status
   */
  static EntityReparentStatus setEntityParent(EntityDatabase &entityDatabase,
                                              Entity entity, Entity parent);

  /**
   * @brief Check if entity can be parented
   *
   * @param entityDatabase Entity database
   * @param entity Entity
   * @param parent Parent
   * @retval true Entity can be parented
   * @retval false Entity cannot be parented
   */
  static EntityReparentStatus
  isValidParentForEntity(EntityDatabase &entityDatabase, Entity entity,
                         Entity parent);
};

} // namespace quoll
