#pragma once

#include "liquid/asset/Asset.h"

namespace liquid {

/**
 * @brief Skinned mesh component
 */
struct SkinnedMeshComponent {
  /**
   * Skinned mesh asset handle
   */
  SkinnedMeshAssetHandle handle = SkinnedMeshAssetHandle::Invalid;
};

} // namespace liquid
