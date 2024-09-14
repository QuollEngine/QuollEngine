#include "quoll/core/Base.h"
#include "quoll/physx/PhysxBackend.h"
#include "PhysicsSystem.h"

namespace quoll {

PhysicsSystem::PhysicsSystem(PhysicsBackend *backend) : mBackend(backend) {}

} // namespace quoll
