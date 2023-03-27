#pragma once

#include "PhysicsObjects.h"

namespace liquid {

/**
 * @brief Collidable component
 */
struct Collidable {
  /**
   * Geometry description
   */
  PhysicsGeometryDesc geometryDesc;

  /**
   * Physics material description
   */
  PhysicsMaterialDesc materialDesc;
};

} // namespace liquid
