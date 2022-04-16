#pragma once

#include "liquid/animation/AnimatorComponent.h"

namespace liquid {

template <class T> struct PrefabComponent {
  uint32_t entity = 0;
  T value{};
};

struct PrefabTransformData {
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;

  int32_t parent = -1;
};

struct PrefabAsset {
  std::vector<PrefabComponent<PrefabTransformData>> transforms;
  std::vector<PrefabComponent<MeshAssetHandle>> meshes;
  std::vector<PrefabComponent<SkinnedMeshAssetHandle>> skinnedMeshes;
  std::vector<PrefabComponent<SkeletonAssetHandle>> skeletons;
  std::vector<PrefabComponent<AnimatorComponent>> animators;
};

} // namespace liquid
