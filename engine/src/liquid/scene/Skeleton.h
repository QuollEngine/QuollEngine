#pragma once

#include "liquid/renderer/ResourceAllocator.h"

namespace liquid {

class Skeleton {
  using JointId = uint8_t;

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
   * @param transform Joint local transform
   * @param parent Joint parent
   * @param inverseBind Joint inverse bind matrix
   * @param name Joint name
   */
  void addJoint(const glm::mat4 &transform, JointId parent,
                const glm::mat4 &inverseBind, const String &name = "");

  /**
   * @brief Get joint local transform
   *
   * @param joint Joint
   * @return Joint local transform
   *
   * @note joint = 0 means root joint
   */
  inline const glm::mat4 &getJointLocalTransform(JointId joint) const {
    LIQUID_ASSERT(joint < jointLocalTransforms.size(), "Joint does not exist");
    return jointLocalTransforms.at(joint);
  }

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
  std::vector<glm::mat4> jointLocalTransforms{glm::mat4{1.0f}};
  std::vector<glm::mat4> jointWorldTransforms{glm::mat4{1.0f}};
  std::vector<glm::mat4> jointInverseBindMatrices{glm::mat4{1.0f}};
  std::vector<String> jointNames{"__rootJoint"};
  SharedPtr<HardwareBuffer> buffer;
};

} // namespace liquid
