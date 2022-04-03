#pragma once

#include "liquid/scene/Joint.h"

namespace liquid {

struct SkeletonAsset {
  std::vector<JointId> jointParents;
  std::vector<glm::vec3> jointLocalPositions;
  std::vector<glm::quat> jointLocalRotations;
  std::vector<glm::vec3> jointLocalScales;
  std::vector<glm::mat4> jointInverseBindMatrices;
  std::vector<String> jointNames;
};

} // namespace liquid
