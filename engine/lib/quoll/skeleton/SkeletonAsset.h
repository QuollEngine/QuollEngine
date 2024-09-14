#pragma once

#include "quoll/skeleton/Joint.h"

namespace quoll {

struct SkeletonAsset {
  std::vector<JointId> jointParents;

  std::vector<glm::vec3> jointLocalPositions;

  std::vector<glm::quat> jointLocalRotations;

  std::vector<glm::vec3> jointLocalScales;

  std::vector<glm::mat4> jointInverseBindMatrices;

  std::vector<String> jointNames;
};

} // namespace quoll
