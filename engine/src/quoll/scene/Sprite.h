#pragma once

#include "quoll/asset/AssetRef.h"
#include "quoll/renderer/TextureAsset.h"

namespace quoll {

struct Sprite {
  AssetRef<TextureAsset> texture;
};

} // namespace quoll
