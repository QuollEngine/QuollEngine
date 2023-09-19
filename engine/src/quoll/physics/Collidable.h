#pragma once

#include "PhysicsObjects.h"

namespace quoll {

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

  /**
   * Use collidable shape in simulation
   */
  bool useInSimulation = true;

  /**
   * Use collidable shape in queries
   */
  bool useInQueries = true;
};

} // namespace quoll
