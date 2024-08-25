#pragma once

#include "quoll/asset/AssetHandle.h"
#include "MaterialAsset.h"

namespace quoll {

struct SkinnedMeshRenderer {
  std::vector<AssetHandle<MaterialAsset>> materials;
};

} // namespace quoll
