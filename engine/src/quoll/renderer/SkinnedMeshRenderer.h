#pragma once

#include "quoll/asset/AssetRef.h"
#include "MaterialAsset.h"

namespace quoll {

struct SkinnedMeshRenderer {
  std::vector<AssetRef<MaterialAsset>> materials;
};

} // namespace quoll
