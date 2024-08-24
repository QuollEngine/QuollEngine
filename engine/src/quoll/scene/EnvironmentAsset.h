#pragma once

#include "quoll/asset/Asset.h"

namespace quoll {

struct EnvironmentAsset {
  TextureAssetHandle irradianceMap = TextureAssetHandle::Null;

  TextureAssetHandle specularMap = TextureAssetHandle::Null;
};

} // namespace quoll
