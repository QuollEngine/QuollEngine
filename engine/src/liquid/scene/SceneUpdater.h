#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityDatabase.h"

namespace quoll {

/**
 * @brief Scene updater
 */
class SceneUpdater {
public:
  /**
   * @brief Updates scene
   *
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);

private:
  /**
   * @brief Update all transforms
   *
   * @param entityDatabase Entity database
   */
  void updateTransforms(EntityDatabase &entityDatabase);

  /**
   * @brief Update all cameras using transforms
   *
   * @param entityDatabase Entity database
   */
  void updateCameras(EntityDatabase &entityDatabase);

  /**
   * @brief Update all lights using transforms
   *
   * @param entityDatabase Entity database
   */
  void updateLights(EntityDatabase &entityDatabase);
};

} // namespace quoll
