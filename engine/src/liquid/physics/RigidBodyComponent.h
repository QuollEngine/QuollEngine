#pragma once

#include <PxConfig.h>
#include <PxRigidActor.h>

namespace liquid {

struct RigidBodyComponent {
  physx::PxRigidActor *actor = nullptr;
};

} // namespace liquid
