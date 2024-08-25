#pragma once

#include "quoll/asset/AssetHandle.h"
#include "MaterialAsset.h"

namespace quoll {

struct MeshRenderer {
  std::vector<AssetHandle<MaterialAsset>> materials;
};

} // namespace quoll
