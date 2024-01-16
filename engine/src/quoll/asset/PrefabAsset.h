#pragma once

#include "quoll/animation/Animator.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/renderer/MeshRenderer.h"
#include "quoll/renderer/SkinnedMeshRenderer.h"

namespace quoll {

template <class TComponentData> struct PrefabComponent {
  u32 entity = 0;

  TComponentData value{};
};

struct PrefabTransformData {
  glm::vec3 position;

  glm::quat rotation;

  glm::vec3 scale;

  i32 parent = -1;
};

struct PrefabAsset {
  std::vector<PrefabComponent<PrefabTransformData>> transforms;

  std::vector<PrefabComponent<MeshAssetHandle>> meshes;

  std::vector<PrefabComponent<SkeletonAssetHandle>> skeletons;

  std::vector<AnimationAssetHandle> animations;

  std::vector<PrefabComponent<AnimatorAssetHandle>> animators;

  std::vector<PrefabComponent<PointLight>> pointLights;

  std::vector<PrefabComponent<DirectionalLight>> directionalLights;

  std::vector<PrefabComponent<String>> names;

  std::vector<PrefabComponent<MeshRenderer>> meshRenderers;

  std::vector<PrefabComponent<SkinnedMeshRenderer>> skinnedMeshRenderers;
};

} // namespace quoll
