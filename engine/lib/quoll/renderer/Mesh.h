#pragma once

#include "quoll/asset/AssetRef.h"
#include "MeshAsset.h"

namespace quoll {

struct Mesh {
  AssetRef<MeshAsset> asset;
};

} // namespace quoll
