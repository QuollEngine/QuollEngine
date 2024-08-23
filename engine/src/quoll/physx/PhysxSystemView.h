#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/Impulse.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/Torque.h"
#include "quoll/scene/WorldTransform.h"
#include "PhysxInstance.h"

namespace quoll {

struct PhysxSystemView {
  flecs::query<> queryNoPhysxInstances;
  flecs::query<Collidable, WorldTransform, PhysxInstance> queryCollidables;
  flecs::query<RigidBody, WorldTransform, PhysxInstance> queryRigidBodies;
  flecs::query<PhysxInstance> queryRigidBodyClears;
  flecs::query<Force, PhysxInstance> queryRigidBodyAppliedForces;
  flecs::query<Impulse, PhysxInstance> queryRigidBodyAppliedImpulses;
  flecs::query<Torque, PhysxInstance> queryRigidBodyAppliedTorques;
};

} // namespace quoll
