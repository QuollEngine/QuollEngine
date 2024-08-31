#pragma once

#include "quoll/asset/AssetRef.h"
#include "quoll/renderer/TextureAsset.h"

namespace quoll {

struct EnvironmentAsset {
  AssetRef<TextureAsset> irradianceMap;

  AssetRef<TextureAsset> specularMap;
};

} // namespace quoll
