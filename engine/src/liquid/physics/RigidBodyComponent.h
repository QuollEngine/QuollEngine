#pragma once

#include <PxConfig.h>
#include <PxRigidDynamic.h>

#include "PhysicsObjects.h"

namespace liquid {

struct RigidBodyComponent {
  PhysicsDynamicRigidBodyDesc dynamicDesc;
  physx::PxRigidDynamic *actor = nullptr;
};

} // namespace liquid
