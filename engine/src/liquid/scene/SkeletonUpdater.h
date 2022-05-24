#pragma once

#include "liquid/entity/EntityContext.h"

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
   * @param entityContext Entity context
   */
  void update(EntityContext &entityContext);
};

} // namespace liquid
