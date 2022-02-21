#pragma once

#include <PxConfig.h>
#include <PxRigidActor.h>
#include <PxShape.h>
#include <PxMaterial.h>

namespace liquid {

struct RigidBodyComponent {
  physx::PxRigidActor *actor = nullptr;
  physx::PxShape *shape = nullptr;
  physx::PxMaterial *material = nullptr;
};

} // namespace liquid
