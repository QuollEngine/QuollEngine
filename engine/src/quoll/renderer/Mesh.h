#pragma once

#include "quoll/asset/AssetHandle.h"
#include "MeshAsset.h"

namespace quoll {

struct Mesh {
  AssetHandle<MeshAsset> handle;
};

} // namespace quoll
