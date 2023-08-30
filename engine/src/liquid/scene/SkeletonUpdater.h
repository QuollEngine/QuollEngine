#pragma once

#include "liquid/entity/EntityDatabase.h"

namespace quoll {

/**
 * @brief Skeleton updater
 *
 * Updates skeleton transforms
 */
class SkeletonUpdater {
public:
  /**
   * @brief Update
   *
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);

private:
  /**
   * @brief Update skeletons
   *
   * @param entityDatabase Entity database
   */
  void updateSkeletons(EntityDatabase &entityDatabase);

  /**
   * @brief Update debug bones
   *
   * @param entityDatabase Entity database
   */
  void updateDebugBones(EntityDatabase &entityDatabase);
};

} // namespace quoll
