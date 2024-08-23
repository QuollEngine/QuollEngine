#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/skeleton/Skeleton.h"

namespace quoll {

struct SkeletonUpdaterSystemView {
  flecs::query<Skeleton> querySkeletons;
};

} // namespace quoll
