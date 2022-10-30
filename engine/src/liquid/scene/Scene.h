#pragma once

#include "liquid/entity/EntityDatabase.h"

namespace liquid {

/**
 * @brief Scene structure
 *
 * Stores entities and metadata about the scene
 */
struct Scene {
  /**
   * Entity database
   */
  EntityDatabase entityDatabase;

  /**
   * Active camera in the scene
   */
  Entity activeCamera = EntityNull;

  /**
   * @brief Dummy camera
   *
   * Used as a fallback if there
   * are no cameras in the scene
   */
  Entity dummyCamera = EntityNull;
};

} // namespace liquid
