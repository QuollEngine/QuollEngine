#pragma once

namespace liquid {

struct EnvironmentAsset {
  TextureAssetHandle irradianceMap;

  TextureAssetHandle specularMap;

  TextureAssetHandle brdfLut;
};

} // namespace liquid
