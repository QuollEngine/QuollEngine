#include "quoll/core/Base.h"
#include "quoll/physics/PhysicsObjects.h"
#include "PhysxMapping.h"
#include <PxConfig.h>
#include <PxPhysicsAPI.h>

using namespace physx;

namespace quoll {

PxGeometryType::Enum
PhysxMapping::getPhysxGeometryType(const PhysicsGeometryType &type) {
  switch (type) {
  case PhysicsGeometryType::Box:
    return PxGeometryType::eBOX;
  case PhysicsGeometryType::Sphere:
    return PxGeometryType::eSPHERE;
  case PhysicsGeometryType::Capsule:
    return PxGeometryType::eCAPSULE;
  case PhysicsGeometryType::Plane:
    return PxGeometryType::ePLANE;
  default:
    return PxGeometryType::eINVALID;
  }
}

PxTransform PhysxMapping::getPhysxTransform(const glm::mat4 &transform) {
  glm::vec3 empty3;
  glm::vec4 empty4;

  glm::vec3 position;
  glm::quat rotation;

  glm::decompose(transform, empty3, rotation, position, empty3, empty4);

  PxTransform pose;
  pose.p.x = position.x;
  pose.p.y = position.y;
  pose.p.z = position.z;

  pose.q.w = rotation.w;
  pose.q.x = rotation.x;
  pose.q.y = rotation.y;
  pose.q.z = rotation.z;

  return pose;
}

PxVec3 PhysxMapping::getPhysxVec3(const glm::vec3 &value) {
  return PxVec3(value.x, value.y, value.z);
}

glm::vec3 PhysxMapping::getVec3(const physx::PxVec3 &value) {
  return glm::vec3(value.x, value.y, value.z);
}

} // namespace quoll
