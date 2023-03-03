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
  Entity activeCamera = Entity::Null;

  /**
   * Dummy camera
   *
   * Used as a fallback if there
   * are no cameras in the scene
   */
  Entity dummyCamera = Entity::Null;

  /**
   * Environment
   */
  Entity environment = Entity::Null;
};

} // namespace liquid
