#pragma once

#include "quoll/entity/EntityDatabase.h"

namespace quoll {

struct Scene {
  EntityDatabase entityDatabase;

  Entity activeCamera;

  /**
   * Dummy camera
   *
   * Used as a fallback if there
   * are no cameras in the scene
   */
  Entity dummyCamera;

  Entity activeEnvironment;

  /**
   * Dummy environment
   *
   * Used as a fallback if there are no
   * environments in the scene
   */
  Entity dummyEnvironment;
};

} // namespace quoll
