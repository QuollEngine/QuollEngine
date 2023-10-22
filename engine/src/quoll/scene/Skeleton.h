#pragma once

#include "quoll/asset/Asset.h"
#include "Joint.h"

namespace quoll {

/**
 * @brief Skeleton component
 *
 * Stores all local and final
 * transforms for the skeleton
 * joints
 */
struct Skeleton {
  /**
   * Number of joints
   */
  u32 numJoints = 0;

  /**
   * List of joint parents
   */
  std::vector<JointId> jointParents;

  /**
   * Joint local positions
   */
  std::vector<glm::vec3> jointLocalPositions;

  /**
   * Joint local rotations
   */
  std::vector<glm::quat> jointLocalRotations;

  /**
   * Joint local scales
   */
  std::vector<glm::vec3> jointLocalScales;

  /**
   * Joint world transforms
   */
  std::vector<glm::mat4> jointWorldTransforms;

  /**
   * Joint inverse bind matrices
   */
  std::vector<glm::mat4> jointInverseBindMatrices;

  /**
   * Joint final transforms
   */
  std::vector<glm::mat4> jointFinalTransforms;

  /**
   * Joint names
   */
  std::vector<String> jointNames;

  /**
   * Skeleton asset
   */
  SkeletonAssetHandle assetHandle;
};

/**
 * @brief Skeleton debug component
 *
 * Stores skeleton debug bone transforms
 * for rendering debug lines
 */
struct SkeletonDebug {
  /**
   * Debug bones
   */
  std::vector<JointId> bones;

  /**
   * Debug bone transforms
   */
  std::vector<glm::mat4> boneTransforms;
};

} // namespace quoll
