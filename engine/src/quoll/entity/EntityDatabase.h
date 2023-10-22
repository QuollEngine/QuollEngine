#pragma once

#include "Entity.h"
#include "EntityStorageSparseSet.h"

namespace quoll {

/**
 * @brief Entity database
 *
 * Default entity database used within the engine
 *
 * Automatically registers engine components but allows
 * register other components for specific purposes.
 */
class EntityDatabase : public EntityStorageSparseSet {
public:
  /**
   * @brief Create entity database
   */
  EntityDatabase();
};

template <class TComponent>
using EntityDatabaseObserver = EntityStorageSparseSetObserver<TComponent>;

} // namespace quoll
