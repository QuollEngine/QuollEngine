#pragma once

#include "liquid/rhi/ResourceRegistry.h"
#include "Joint.h"

namespace liquid {

class Skeleton {
public:
  /**
   * @brief Create skeleton
   *
   * @param positions Joint positions
   * @param rotations Joint rotations
   * @param scales Joint scales
   * @param parents Joint parents
   * @param inverseBindMatrices Joint inverse bind matrices
   * @param names Joint names
   * @param registry Resource registry
   */
  Skeleton(std::vector<glm::vec3> &&positions,
           std::vector<glm::quat> &&rotations, std::vector<glm::vec3> &&scales,
           std::vector<JointId> &&parents,
           std::vector<glm::mat4> &&inverseBindMatrices,
           std::vector<String> &&names,
           experimental::ResourceRegistry *registry);

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
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
    return jointLocalPositions.at(joint);
  }

  /**
   * @brief Get joint local rotation
   *
   * @param joint Joint
   * @return Joint local scale
   */
  inline const glm::quat &getLocalRotation(JointId joint) const {
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
    return jointLocalRotations.at(joint);
  }

  /**
   * @brief Get joint local scale
   *
   * @param joint Joint
   * @return Joint local scale
   */
  inline const glm::vec3 &getLocalScale(JointId joint) const {
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
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
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
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
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
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
  inline JointId getJointParent(JointId joint) const {
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
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
  inline const String &getJointName(JointId joint) const {
    LIQUID_ASSERT(joint < numJoints, "Joint does not exist");
    return jointNames.at(joint);
  }

  /**
   * @brief Get uniform buffer
   *
   * @return Uniform buffer
   */
  inline BufferHandle getBuffer() const { return buffer; }

  /**
   * @brief Get debug uniform buffer
   *
   * This buffer does not have inverse
   * bind matrices applied to it
   *
   * @return Debug uniform buffer
   */
  inline BufferHandle getDebugBuffer() const { return debugBuffer; }

  /**
   * @brief Get number of joints
   *
   * @return Number of joints
   */
  inline size_t getNumJoints() const { return jointParents.size(); }

  /**
   * @brief Get number of bones
   *
   * @used for debugging purposes only
   *
   * @return Number for bones
   */
  inline size_t getNumDebugBones() const { return debugBoneTransforms.size(); }

  /**
   * @brief Update skeleton
   */
  void update();

  /**
   * @brief Update debug data
   *
   * Calculates bone transforms
   * and updates debug buffer
   */
  void updateDebug();

private:
  size_t numJoints = 0;
  std::vector<JointId> jointParents;
  std::vector<glm::vec3> jointLocalPositions;
  std::vector<glm::quat> jointLocalRotations;
  std::vector<glm::vec3> jointLocalScales;

  std::vector<glm::mat4> jointWorldTransforms;
  std::vector<glm::mat4> jointInverseBindMatrices;
  std::vector<glm::mat4> jointFinalTransforms;

  std::vector<String> jointNames;
  BufferHandle buffer;

  std::vector<JointId> debugBones;
  std::vector<glm::mat4> debugBoneTransforms;
  BufferHandle debugBuffer;

  experimental::ResourceRegistry *registry;
};

} // namespace liquid
