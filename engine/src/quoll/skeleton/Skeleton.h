#pragma once

#include "quoll/asset/AssetRef.h"
#include "Joint.h"
#include "SkeletonAsset.h"

namespace quoll {

struct Skeleton {
  u32 numJoints = 0;

  std::vector<JointId> jointParents;

  std::vector<glm::vec3> jointLocalPositions;

  std::vector<glm::quat> jointLocalRotations;

  std::vector<glm::vec3> jointLocalScales;

  std::vector<glm::mat4> jointWorldTransforms;

  std::vector<glm::mat4> jointInverseBindMatrices;

  std::vector<glm::mat4> jointFinalTransforms;

  std::vector<String> jointNames;

  AssetRef<SkeletonAsset> assetHandle;
};

struct SkeletonDebug {
  std::vector<JointId> bones;

  std::vector<glm::mat4> boneTransforms;
};

} // namespace quoll
