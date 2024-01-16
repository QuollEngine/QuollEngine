#pragma once

#include "quoll/entity/EntityDatabase.h"

namespace quoll {

class SkeletonUpdater {
public:
  void update(EntityDatabase &entityDatabase);

private:
  void updateSkeletons(EntityDatabase &entityDatabase);

  void updateDebugBones(EntityDatabase &entityDatabase);
};

} // namespace quoll
