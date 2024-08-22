#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "PhysxInstance.h"

namespace quoll {

struct PhysxSystemView {
  EntityDatabaseObserver<PhysxInstance> instanceRemoveObserver;
};

} // namespace quoll
