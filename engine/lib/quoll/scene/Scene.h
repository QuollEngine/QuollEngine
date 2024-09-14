#pragma once

#include "quoll/entity/EntityDatabase.h"

namespace quoll {

struct Scene {
  EntityDatabase entityDatabase;

  Entity activeCamera = Entity::Null;

  /**
   * Dummy camera
   *
   * Used as a fallback if there
   * are no cameras in the scene
   */
  Entity dummyCamera = Entity::Null;

  Entity activeEnvironment = Entity::Null;

  /**
   * Dummy environment
   *
   * Used as a fallback if there are no
   * environments in the scene
   */
  Entity dummyEnvironment = Entity::Null;
};

} // namespace quoll
