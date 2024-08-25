#pragma once

#include "quoll/asset/AssetHandle.h"
#include "quoll/renderer/TextureAsset.h"

namespace quoll {

struct Sprite {
  AssetHandle<TextureAsset> handle;
};

} // namespace quoll
