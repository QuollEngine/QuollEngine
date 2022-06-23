#pragma once

#include "liquid/entity/EntityDatabase.h"

namespace liquid {

/**
 * @brief Skeleton updater
 *
 * Updates skeleton transforms
 */
class SkeletonUpdater {
public:
  /**
   * @brief Update skeletons
   *
   * Update skeleton joint transforms and
   * debug skeleton bone transforms
   *
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);
};

} // namespace liquid
