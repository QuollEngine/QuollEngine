#pragma once

#include "liquid/asset/Asset.h"

namespace liquid {

/**
 * @brief Mesh component
 */
struct MeshComponent {
  /**
   * Mesh asset handle
   */
  MeshAssetHandle handle = MeshAssetHandle::Invalid;
};

} // namespace liquid
