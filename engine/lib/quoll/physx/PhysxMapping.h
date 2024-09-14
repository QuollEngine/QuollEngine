#pragma once

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

namespace quoll {

class PhysxMapping {
public:
  static physx::PxGeometryType::Enum
  getPhysxGeometryType(const PhysicsGeometryType &type);

  static physx::PxTransform getPhysxTransform(const glm::mat4 &transform);

  static physx::PxVec3 getPhysxVec3(const glm::vec3 &value);

  static glm::vec3 getVec3(const physx::PxVec3 &value);
};

} // namespace quoll
