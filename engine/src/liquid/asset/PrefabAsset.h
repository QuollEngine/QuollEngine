#pragma once

#include "liquid/animation/AnimatorComponent.h"

namespace liquid {

/**
 * @brief Prefab component
 *
 * @tparam TValue value type
 */
template <class T> struct PrefabComponent {
  /**
   * Local entity ID
   *
   * This value is local to prefab asset file
   */
  uint32_t entity = 0;

  /**
   * Value
   */
  T value{};
};

/**
 * @brief Prefab transforms
 */
struct PrefabTransformData {
  /**
   * Local position
   */
  glm::vec3 position;

  /**
   * Local rotation
   */
  glm::quat rotation;

  /**
   * Local scale
   */
  glm::vec3 scale;

  /**
   * Parent ID
   *
   * This value is local to prefab asset file
   */
  int32_t parent = -1;
};

/**
 * @brief Prefab asset data
 */
struct PrefabAsset {
  /**
   * List of transforms
   */
  std::vector<PrefabComponent<PrefabTransformData>> transforms;

  /**
   * List of meshes
   */
  std::vector<PrefabComponent<MeshAssetHandle>> meshes;

  /**
   * List of skinned meshes
   */
  std::vector<PrefabComponent<SkinnedMeshAssetHandle>> skinnedMeshes;

  /**
   * List of skeletons
   */
  std::vector<PrefabComponent<SkeletonAssetHandle>> skeletons;

  /**
   * List of animators
   */
  std::vector<PrefabComponent<AnimatorComponent>> animators;
};

} // namespace liquid
