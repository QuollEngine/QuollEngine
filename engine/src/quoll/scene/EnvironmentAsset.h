#pragma once

#include "quoll/asset/AssetHandle.h"
#include "quoll/renderer/TextureAsset.h"

namespace quoll {

struct EnvironmentAsset {
  AssetHandle<TextureAsset> irradianceMap;

  AssetHandle<TextureAsset> specularMap;
};

} // namespace quoll
