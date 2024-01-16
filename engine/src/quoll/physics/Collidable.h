#pragma once

#include "PhysicsObjects.h"

namespace quoll {

struct Collidable {
  PhysicsGeometryDesc geometryDesc;

  PhysicsMaterialDesc materialDesc;

  bool useInSimulation = true;

  bool useInQueries = true;
};

} // namespace quoll
