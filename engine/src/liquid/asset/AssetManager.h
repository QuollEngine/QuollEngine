#pragma once

#include "AssetRegistry.h"

namespace liquid {

class AssetManager {
public:
  /**
   * @brief Create asset manager
   *
   * @param assetsPath Assets path
   */
  AssetManager(const std::filesystem::path &assetsPath);

  /**
   * @brief Create texture from asset
   *
   * Create engine specific texture
   * asset from any texture asset
   *
   * @param asset Texture asset
   * @return Path to new texture asset
   */
  std::filesystem::path
  createTextureFromAsset(const AssetData<TextureAsset> &asset);

  /**
   * @brief Load texture from file
   *
   * @param filePath Path to asset
   * @return Texture asset handle
   */
  TextureAssetHandle loadTextureFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Get asset registry
   *
   * @return Asset registry
   */
  inline AssetRegistry &getRegistry() { return mRegistry; }

private:
  AssetRegistry mRegistry;
  std::filesystem::path mAssetsPath;
};

} // namespace liquid
