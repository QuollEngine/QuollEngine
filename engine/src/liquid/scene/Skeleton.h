#pragma once

#include "liquid/renderer/ResourceAllocator.h"
#include "Joint.h"

namespace liquid {

class Skeleton {
public:
  /**
   * @brief Create skeleton
   *
   * @param numJoints Number of joints
   * @param resourseAllocator resoursse Allocator
   */
  Skeleton(uint32_t numJoints, ResourceAllocator *resourseAllocator);

  /**
   * @brief Adds joint
   *
   * @param position Joint local position
   * @param rotation Joint local rotation
   * @param scale Joint local scale
   * @param parent Joint parent
   * @param inverseBind Joint inverse bind matrix
   * @param name Joint name
   */
  void addJoint(const glm::vec3 &position, const glm::quat &rotation,
                const glm::vec3 scale, JointId parent,
                const glm::mat4 &inverseBind, const String &name = "");

  /**
   * @brief Set joint position
   *
   * @param joint Joint
   * @param position Local position
   */
  void setJointPosition(JointId joint, const glm::vec3 &position);

  /**
   * @brief Set joint rotation
   *
   * @param joint Joint
   * @param rotation Local rotation
   */
  void setJointRotation(JointId joint, const glm::quat &rotation);

  /**
   * @brief Set joint scale
   *
   * @param joint Joint
   * @param scale Local scale
   */
  void setJointScale(JointId joint, const glm::vec3 &scale);

  /**
   * @brief Get joint local position
   *
   * @param joint Joint
   * @return Joint local position
   */
  inline const glm::vec3 &getLocalPosition(JointId joint) const {
    LIQUID_ASSERT(joint < jointLocalPositions.size(), "Joint does not exist");
    return jointLocalPositions.at(joint);
  }

  /**
   * @brief Get joint local rotation
   *
   * @param joint Joint
   * @return Joint local scale
   */
  inline const glm::quat &getLocalRotation(JointId joint) const {
    LIQUID_ASSERT(joint < jointLocalRotations.size(), "Joint does not exist");
    return jointLocalRotations.at(joint);
  }

  /**
   * @brief Get joint local scale
   *
   * @param joint Joint
   * @return Joint local scale
   */
  inline const glm::vec3 &getLocalScale(JointId joint) const {
    LIQUID_ASSERT(joint < jointLocalScales.size(), "Joint does not exist");
    return jointLocalScales.at(joint);
  }

  /**
   * @brief Get joint local transform
   *
   * @param joint Joint
   * @return Joint local transform
   *
   * @note joint = 0 means root joint
   */
  const glm::mat4 getJointLocalTransform(JointId joint) const;

  /**
   * @brief Get joint world transform
   *
   * @param joint Joint
   * @return Joint world transform
   *
   * @note joint = 0 means root joint
   */
  inline const glm::mat4 &getJointWorldTransform(JointId joint) const {
    LIQUID_ASSERT(joint < jointWorldTransforms.size(), "Joint does not exist");
    return jointWorldTransforms.at(joint);
  }

  /**
   * @brief Get joint inverse bind matrix
   *
   * @param joint Joint
   * @return Joint inverse bind matrix
   *
   * @note joint = 0 means root joint
   */
  inline const glm::mat4 &getJointInverseBindMatrix(JointId joint) const {
    LIQUID_ASSERT(joint < jointInverseBindMatrices.size(),
                  "Joint does not exist");
    return jointInverseBindMatrices.at(joint);
  }

  /**
   * @brief Get joint parent
   *
   * @param joint Joint
   * @return Joint parent
   *
   * @note joint = 0 means root joint
   */
  inline JointId getJointParent(JointId joint) {
    LIQUID_ASSERT(joint < jointParents.size(), "Joint does not exist");
    return jointParents.at(joint);
  }

  /**
   * @brief Get joint name
   *
   * @param joint Joint
   * @return Joint name
   *
   * @note joint = 0 means root joint
   */
  inline const String &getJointName(JointId joint) {
    LIQUID_ASSERT(joint < jointNames.size(), "Joint does not exist");
    return jointNames.at(joint);
  }

  /**
   * @brief Get uniform buffer
   *
   * @return Uniform buffer
   */
  inline const SharedPtr<HardwareBuffer> &getBuffer() const { return buffer; }

  /**
   * @brief Update skeleton
   */
  void update();

private:
  size_t numJoints = 0;
  std::vector<JointId> jointParents{0};
  std::vector<glm::vec3> jointLocalPositions{glm::vec3{0.0f}};
  std::vector<glm::quat> jointLocalRotations{glm::quat{1.0f, 0.0f, 0.0f, 0.0f}};
  std::vector<glm::vec3> jointLocalScales{glm::vec3{1.0f}};

  std::vector<glm::mat4> jointWorldTransforms{glm::mat4{1.0f}};
  std::vector<glm::mat4> jointInverseBindMatrices{glm::mat4{1.0f}};
  std::vector<String> jointNames{"__rootJoint"};
  SharedPtr<HardwareBuffer> buffer;
};

} // namespace liquid
