#pragma once

#include "liquid/asset/Asset.h"

namespace liquid {

/**
 * @brief Skinned mesh component
 */
struct SkinnedMesh {
  /**
   * Skinned mesh asset handle
   */
  SkinnedMeshAssetHandle handle = SkinnedMeshAssetHandle::Null;
};

} // namespace liquid
