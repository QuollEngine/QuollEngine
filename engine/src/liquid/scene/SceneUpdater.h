#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

class SceneUpdater {
public:
  /**
   * @brief Updates scene
   */
  void update(EntityContext &entityContext);

private:
  /**
   * @brief Update all transforms
   *
   * @param entityContext Entity context
   */
  void updateTransforms(EntityContext &entityContext);

  /**
   * @brief Update all cameras using transforms
   *
   * @param entityContext Entity context
   */
  void updateCameras(EntityContext &entityContext);

  /**
   * @brief Update all lights using transforms
   *
   * @param entityContext Entity context
   */
  void updateLights(EntityContext &entityContext);
};

} // namespace liquid
