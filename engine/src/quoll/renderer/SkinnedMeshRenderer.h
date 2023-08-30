#pragma once

namespace quoll {

/**
 * @brief Skinned mesh renderer component
 *
 * Used to render skinned meshes
 */
struct SkinnedMeshRenderer {
  /**
   * Materials
   */
  std::vector<MaterialAssetHandle> materials;
};

} // namespace quoll
