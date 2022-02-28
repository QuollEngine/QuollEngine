#pragma once

#include <PxConfig.h>
#include <PxShape.h>
#include <PxMaterial.h>

namespace liquid {

struct CollidableComponent {
  physx::PxShape *shape = nullptr;
  physx::PxMaterial *material = nullptr;
  physx::PxRigidStatic *rigidStatic = nullptr;
};

} // namespace liquid
