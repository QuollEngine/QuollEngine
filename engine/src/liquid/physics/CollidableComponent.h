#pragma once

#include <PxConfig.h>
#include <PxShape.h>
#include <PxMaterial.h>

#include "PhysicsObjects.h"

namespace liquid {

struct CollidableComponent {
  PhysicsGeometryDesc geometryDesc;
  PhysicsMaterialDesc materialDesc;

  physx::PxShape *shape = nullptr;
  physx::PxMaterial *material = nullptr;
  physx::PxRigidStatic *rigidStatic = nullptr;
};

} // namespace liquid
