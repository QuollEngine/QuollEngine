#pragma once

namespace quoll {

/**
 * @brief Maps physx data with engine
 * specific data
 */
class PhysxMapping {
public:
  /**
   * @brief Get Physx geometry type
   *
   * @param type Engine physics geometry type
   * @return Physx geometry type
   */
  static physx::PxGeometryType::Enum
  getPhysxGeometryType(const PhysicsGeometryType &type);

  /**
   * @brief Get Physx transform
   *
   * @param transform Transform matrix
   * @return Physx transform
   */
  static physx::PxTransform getPhysxTransform(const glm::mat4 &transform);

  /**
   * @brief Get Physx vec3
   *
   * @param value Vec3 value
   * @return Physx vec3
   */
  static physx::PxVec3 getPhysxVec3(const glm::vec3 &value);

  /**
   * @brief Get Vec3
   *
   * @param value Physx vec3
   * @return Vec3 value
   */
  static glm::vec3 getVec3(const physx::PxVec3 &value);
};

} // namespace quoll
