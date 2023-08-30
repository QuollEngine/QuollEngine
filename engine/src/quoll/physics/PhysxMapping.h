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
};

} // namespace quoll
