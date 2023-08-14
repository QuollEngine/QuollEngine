#pragma once

namespace liquid {

/**
 * @brief Mesh renderer component
 *
 * Used to render meshes
 */
struct MeshRenderer {
  /**
   * Materials
   */
  std::vector<MaterialAssetHandle> materials;
};

} // namespace liquid
