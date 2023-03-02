#pragma once

namespace liquid {

/**
 * @brief Environment asset
 */
struct EnvironmentAsset {
  /**
   * Irradiance map
   */
  TextureAssetHandle irradianceMap = TextureAssetHandle::Invalid;

  /**
   * Specular map
   */
  TextureAssetHandle specularMap = TextureAssetHandle::Invalid;
};

} // namespace liquid
