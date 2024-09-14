#pragma once

#include "Entity.h"
#include "EntityStorageSparseSet.h"

namespace quoll {

class EntityDatabase : public EntityStorageSparseSet {
public:
  EntityDatabase();
};

template <class TComponent>
using EntityDatabaseObserver = EntityStorageSparseSetObserver<TComponent>;

} // namespace quoll
