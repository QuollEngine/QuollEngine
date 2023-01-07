#pragma once

#include "liquid/asset/AssetCache.h"

namespace liquidator {

/**
 * @brief Manage all the assets in the editor
 *
 * - Import original assets by creating engine assets
 * - Validate assets for changes
 * - Preload assets
 * - Creates assets (e.g script files)
 *
 * Nomeclature:
 *
 * assets directory - Directory of original assets
 * cache directory - Directory of engine specific assets and hash files
 * original asset - Original assets (stored in assets directory)
 * engine asset - Transformed assets that
 *   can be used within the engine (stored in cache directory)
 * hash file - Stores hashes of both original and
 *   engine assets (stored in cache directory)
 *
 * @warning DO NOT PROVIDE RELATIVE PATH TO ANY FUNCTION
 *          IN THIS OBJECT!
 *
 * All the functions in this object **only** work
 * with absolute paths.
 */
class AssetManager {
public:
  /**
   * @brief Supported texture extensions
   */
  static const std::vector<liquid::String> TextureExtensions;

  /**
   * @brief Supported script extensions
   */
  static const std::vector<liquid::String> ScriptExtensions;

  /**
   * @brief Supported audio extensions
   */
  static const std::vector<liquid::String> AudioExtensions;

  /**
   * @brief Supported font extensions
   */
  static const std::vector<liquid::String> FontExtensions;

  /**
   * @brief Supported scene extensions
   */
  static const std::vector<liquid::String> SceneExtensions;

public:
  /**
   * @brief Create asset manager
   *
   * @param assetsPath Path to assets directory
   * @param assetsCachePath Path to assets cache directory
   * @param createDefaultObjects Create default objects
   */
  AssetManager(const liquid::Path &assetsPath,
               const liquid::Path &assetsCachePath,
               bool createDefaultObjects = true);

  /**
   * @brief Import asset
   *
   * @param source Path to source asset
   * @param targetAssetDirectory Target directory
   * @return Import result
   */
  liquid::Result<bool> importAsset(const liquid::Path &source,
                                   const liquid::Path &targetAssetDirectory);

  /**
   * @brief Get asset registry
   *
   * @return Asset registry
   */
  inline liquid::AssetRegistry &getAssetRegistry() {
    return mAssetCache.getRegistry();
  }

  /**
   * @brief Get path to assets
   *
   * @return Path to assets
   */
  inline const liquid::Path &getAssetsPath() const { return mAssetsPath; }

  /**
   * @brief Get path to cache
   *
   * @return Path to cache
   */
  inline const liquid::Path &getCachePath() const {
    return mAssetCache.getAssetsPath();
  }

  /**
   * @brief Find engine asset path
   *
   * @param originalAssetPath Original asset path
   * @return Found engine asset path result
   */
  liquid::Path findEngineAssetPath(const liquid::Path &originalAssetPath);

  /**
   * @brief Create directory in assets
   *
   * @param assetsPath Path to directory in assets
   * @return Result
   */
  liquid::Result<bool> createDirectory(const liquid::Path &assetsPath);

  /**
   * @brief Create Lua script in assets
   *
   * @param assetPath Path to assets
   * @return Result
   */
  liquid::Result<liquid::Path> createLuaScript(const liquid::Path &assetPath);

  /**
   * @brief Validate and preload assets
   *
   * @param renderStorage Render storage
   * @return Result
   */
  liquid::Result<bool>
  validateAndPreloadAssets(liquid::RenderStorage &renderStorage);

  /**
   * @brief Load original asset if hashes are changed
   *
   * @param originalAssetPath Original asset path
   * @return Result
   */
  liquid::Result<bool>
  loadOriginalIfChanged(const liquid::Path &originalAssetPath);

private:
  /**
   * @brief Get asset type from path extension
   *
   * @param path Path to file
   * @return Asset type
   */
  static liquid::AssetType getAssetTypeFromExtension(const liquid::Path &path);

  /**
   * @brief Get hash of a file
   *
   * @param path Path to file
   * @return File hash
   */
  static liquid::String getFileHash(const liquid::Path &path);

  /**
   * @brief Recursively create directories
   *
   * @param path Path to target directory
   * @return List of directories that were created
   */
  static std::optional<liquid::Path>
  createDirectoriesRecursive(const liquid::Path &path);

private:
  /**
   * @brief Convert to cache relative asset path
   *
   * @param originalAssetPath Original asset path
   * @return Cache relative asset path
   */
  liquid::Path
  convertToCacheRelativePath(const liquid::Path &originalAssetPath) const;

  /**
   * @brief Get hash file path from original asset
   *
   * @param originalAssetPath Original asset path
   * @return Hash file path
   */
  liquid::Path getHashFilePath(const liquid::Path &originalAssetPath) const;

  /**
   * @brief Get original asset name
   *
   * @param originalAssetPath Original asset path
   * @return Original asset name
   */
  liquid::String getOriginalAssetName(const liquid::Path &originalAssetPath);

  /**
   * @brief Check if asset is changed
   *
   * A changed asset means the following:
   *
   * - Original asset file is changed
   * - Engine file does not exist for the asset
   * - Hash file does not exist for the asset
   *
   * @param path Original asset path
   * @retval true Asset is changed
   * @retval false Asset is not changed
   */
  bool isAssetChanged(const liquid::Path &originalAssetPath) const;

  /**
   * @brief Create hash file
   *
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @return Path to newly created hash file
   */
  liquid::Result<liquid::Path>
  createHashFile(const liquid::Path &originalAssetPath,
                 const liquid::Path &engineAssetPath);

private:
  /**
   * @brief Load original asset
   *
   * @param originalAssetPath Original asset path
   * @return Load result
   */
  liquid::Result<bool> loadOriginalAsset(const liquid::Path &originalAssetPath);

  /**
   * @brief Load texture asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for texture result
   */
  liquid::Result<liquid::Path>
  loadOriginalTexture(const liquid::Path &originalAssetPath);

  /**
   * @brief Load audio asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for audio result
   */
  liquid::Result<liquid::Path>
  loadOriginalAudio(const liquid::Path &originalAssetPath);

  /**
   * @brief Load script asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for script result
   */
  liquid::Result<liquid::Path>
  loadOriginalScript(const liquid::Path &originalAssetPath);

  /**
   * @brief Load font asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for font result
   */
  liquid::Result<liquid::Path>
  loadOriginalFont(const liquid::Path &originalAssetPath);

  /**
   * @brief Load prefab asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for prefab result
   */
  liquid::Result<liquid::Path>
  loadOriginalPrefab(const liquid::Path &originalAssetPath);

private:
  liquid::AssetCache mAssetCache;
  liquid::Path mAssetsPath;

  std::unordered_map<liquid::String, liquid::Path> mAssetCacheMap;
};

} // namespace liquidator
