#pragma once

#include "quoll/entity/EntityDatabase.h"

namespace quoll {

enum class EntityReparentStatus {
  Ok,
  ParentHasNotChanged,
  CannotParentEntityToDescendant
};

class EntityRelationUtils {
public:
  static void removeEntityParent(EntityDatabase &entityDatabase, Entity entity);

  static EntityReparentStatus setEntityParent(EntityDatabase &entityDatabase,
                                              Entity entity, Entity parent);

  static EntityReparentStatus
  isValidParentForEntity(EntityDatabase &entityDatabase, Entity entity,
                         Entity parent);
};

} // namespace quoll
