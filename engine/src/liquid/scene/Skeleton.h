#pragma once

#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/asset/Asset.h"
#include "Joint.h"

namespace liquid {

/**
 * @brief Skeleton instance
 */
class Skeleton {
public:
  /**
   * @brief Create skeleton
   *
   * @param assetHandle Skeleton asset handle
   * @param positions Joint positions
   * @param rotations Joint rotations
   * @param scales Joint scales
   * @param parents Joint parents
   * @param inverseBindMatrices Joint inverse bind matrices
   * @param names Joint names
   * @param registry Resource registry
   */
  Skeleton(SkeletonAssetHandle assetHandle,
           const std::vector<glm::vec3> &positions,
           const std::vector<glm::quat> &rotations,
           const std::vector<glm::vec3> &scales,
           const std::vector<JointId> &parents,
           const std::vector<glm::mat4> &inverseBindMatrices,
           const std::vector<String> &names, rhi::ResourceRegistry *registry);

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
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointLocalPositions.at(joint);
  }

  /**
   * @brief Get joint local rotation
   *
   * @param joint Joint
   * @return Joint local scale
   */
  inline const glm::quat &getLocalRotation(JointId joint) const {
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointLocalRotations.at(joint);
  }

  /**
   * @brief Get joint local scale
   *
   * @param joint Joint
   * @return Joint local scale
   */
  inline const glm::vec3 &getLocalScale(JointId joint) const {
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointLocalScales.at(joint);
  }

  /**
   * @brief Get asset handle
   *
   * @return Skeleton asset handle
   */
  inline SkeletonAssetHandle getAssetHandle() const { return mAssetHandle; }

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
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointWorldTransforms.at(joint);
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
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointInverseBindMatrices.at(joint);
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
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointParents.at(joint);
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
    LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");
    return mJointNames.at(joint);
  }

  /**
   * @brief Get joint final transforms
   *
   * @return Joint final transforms
   */
  inline const std::vector<glm::mat4> &getJointFinalTransforms() const {
    return mJointFinalTransforms;
  }

  /**
   * @brief Get debug buffer
   *
   * This buffer does not have inverse
   * bind matrices applied to it
   *
   * @return Debug buffer
   */
  inline rhi::BufferHandle getDebugBuffer() const { return mDebugBuffer; }

  /**
   * @brief Get number of joints
   *
   * @return Number of joints
   */
  inline size_t getNumJoints() const { return mJointParents.size(); }

  /**
   * @brief Get number of bones
   *
   * Used for debugging purposes only
   *
   * @return Number for bones
   */
  inline uint32_t getNumDebugBones() const {
    return static_cast<uint32_t>(mDebugBoneTransforms.size());
  }

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
  size_t mNumJoints = 0;
  std::vector<JointId> mJointParents;
  std::vector<glm::vec3> mJointLocalPositions;
  std::vector<glm::quat> mJointLocalRotations;
  std::vector<glm::vec3> mJointLocalScales;

  std::vector<glm::mat4> mJointWorldTransforms;
  std::vector<glm::mat4> mJointInverseBindMatrices;
  std::vector<glm::mat4> mJointFinalTransforms;

  std::vector<String> mJointNames;

  std::vector<JointId> mDebugBones;
  std::vector<glm::mat4> mDebugBoneTransforms;
  rhi::BufferHandle mDebugBuffer;

  rhi::ResourceRegistry *mRegistry;

  SkeletonAssetHandle mAssetHandle;
};

} // namespace liquid
