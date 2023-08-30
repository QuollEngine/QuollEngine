#include "quoll/core/Base.h"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>
#include "PhysicsObjects.h"
#include "PhysxMapping.h"

namespace quoll {

physx::PxGeometryType::Enum
PhysxMapping::getPhysxGeometryType(const PhysicsGeometryType &type) {
  switch (type) {
  case PhysicsGeometryType::Box:
    return physx::PxGeometryType::eBOX;
  case PhysicsGeometryType::Sphere:
    return physx::PxGeometryType::eSPHERE;
  case PhysicsGeometryType::Capsule:
    return physx::PxGeometryType::eCAPSULE;
  case PhysicsGeometryType::Plane:
    return physx::PxGeometryType::ePLANE;
  default:
    return physx::PxGeometryType::eINVALID;
  }
}

physx::PxTransform PhysxMapping::getPhysxTransform(const glm::mat4 &transform) {
  glm::vec3 empty3;
  glm::vec4 empty4;

  glm::vec3 position;
  glm::quat rotation;

  glm::decompose(transform, empty3, rotation, position, empty3, empty4);

  physx::PxTransform pose;
  pose.p.x = position.x;
  pose.p.y = position.y;
  pose.p.z = position.z;

  pose.q.w = rotation.w;
  pose.q.x = rotation.x;
  pose.q.y = rotation.y;
  pose.q.z = rotation.z;

  return pose;
}

} // namespace quoll
