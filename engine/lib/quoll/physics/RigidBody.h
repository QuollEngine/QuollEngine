#pragma once

#include "PhysicsObjects.h"

namespace quoll {

enum class RigidBodyType { Dynamic, Kinematic };

struct RigidBody {
  RigidBodyType type{RigidBodyType::Dynamic};

  PhysicsDynamicRigidBodyDesc dynamicDesc;
};

} // namespace quoll
