#pragma once

#include "liquid/asset/AssetCache.h"
#include "ImageLoader.h"
#include "HDRIImporter.h"

namespace liquid::editor {

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
  static const std::vector<String> TextureExtensions;

  /**
   * @brief Supported script extensions
   */
  static const std::vector<String> ScriptExtensions;

  /**
   * @brief Supported audio extensions
   */
  static const std::vector<String> AudioExtensions;

  /**
   * @brief Supported font extensions
   */
  static const std::vector<String> FontExtensions;

  /**
   * @brief Supported scene extensions
   */
  static const std::vector<String> SceneExtensions;

  /**
   * @brief Supported environment extensions
   */
  static const std::vector<String> EnvironmentExtensions;

public:
  /**
   * @brief Create asset manager
   *
   * @param assetsPath Path to assets directory
   * @param assetsCachePath Path to assets cache directory
   * @param device Render device for computing
   * @param optimize Optimize assets
   * @param createDefaultObjects Create default objects
   */
  AssetManager(const Path &assetsPath, const Path &assetsCachePath,
               liquid::rhi::RenderDevice *device, bool optimize,
               bool createDefaultObjects);

  /**
   * @brief Import asset
   *
   * @param source Path to source asset
   * @param targetAssetDirectory Target directory
   * @return Path to imported original asset
   */
  Result<Path> importAsset(const Path &source,
                           const Path &targetAssetDirectory);

  /**
   * @brief Get asset registry
   *
   * @return Asset registry
   */
  inline AssetRegistry &getAssetRegistry() { return mAssetCache.getRegistry(); }

  /**
   * @brief Get path to assets
   *
   * @return Path to assets
   */
  inline const Path &getAssetsPath() const { return mAssetsPath; }

  /**
   * @brief Get path to cache
   *
   * @return Path to cache
   */
  inline const Path &getCachePath() const {
    return mAssetCache.getAssetsPath();
  }

  /**
   * @brief Generate preview
   *
   * @param path Original asset path
   * @param renderStorage Render storage
   */
  void generatePreview(const Path &path, RenderStorage &renderStorage);

  /**
   * @brief Find engine asset path
   *
   * @param originalAssetPath Original asset path
   * @return Found engine asset path result
   */
  Path findEngineAssetPath(const Path &originalAssetPath);

  /**
   * @brief Create directory in assets
   *
   * @param assetsPath Path to directory in assets
   * @return Result
   */
  Result<bool> createDirectory(const Path &assetsPath);

  /**
   * @brief Create Lua script in assets
   *
   * @param assetPath Path to assets
   * @return Result
   */
  Result<Path> createLuaScript(const Path &assetPath);

  /**
   * @brief Validate and preload assets
   *
   * @param renderStorage Render storage
   * @return Result
   */
  Result<bool> validateAndPreloadAssets(RenderStorage &renderStorage);

  /**
   * @brief Load original asset if hashes are changed
   *
   * @param originalAssetPath Original asset path
   * @return Result
   */
  Result<bool> loadOriginalIfChanged(const Path &originalAssetPath);

private:
  /**
   * @brief Get asset type from path extension
   *
   * @param path Path to file
   * @return Asset type
   */
  static AssetType getAssetTypeFromExtension(const Path &path);

  /**
   * @brief Get hash of a file
   *
   * @param path Path to file
   * @return File hash
   */
  static String getFileHash(const Path &path);

  /**
   * @brief Recursively create directories
   *
   * @param path Path to target directory
   * @return List of directories that were created
   */
  static std::optional<Path> createDirectoriesRecursive(const Path &path);

private:
  /**
   * @brief Convert to cache relative asset path
   *
   * @param originalAssetPath Original asset path
   * @return Cache relative asset path
   */
  Path convertToCacheRelativePath(const Path &originalAssetPath) const;

  /**
   * @brief Get hash file path from original asset
   *
   * @param originalAssetPath Original asset path
   * @return Hash file path
   */
  Path getHashFilePath(const Path &originalAssetPath) const;

  /**
   * @brief Get original asset name
   *
   * @param originalAssetPath Original asset path
   * @return Original asset name
   */
  String getOriginalAssetName(const Path &originalAssetPath);

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
  bool isAssetChanged(const Path &originalAssetPath) const;

  /**
   * @brief Create hash file
   *
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @return Path to newly created hash file
   */
  Result<Path> createHashFile(const Path &originalAssetPath,
                              const Path &engineAssetPath);

private:
  /**
   * @brief Load original asset
   *
   * @param originalAssetPath Original asset path
   * @return Load result
   */
  Result<bool> loadOriginalAsset(const Path &originalAssetPath);

  /**
   * @brief Load texture asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for texture result
   */
  Result<Path> loadOriginalTexture(const Path &originalAssetPath);

  /**
   * @brief Load audio asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for audio result
   */
  Result<Path> loadOriginalAudio(const Path &originalAssetPath);

  /**
   * @brief Load script asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for script result
   */
  Result<Path> loadOriginalScript(const Path &originalAssetPath);

  /**
   * @brief Load font asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for font result
   */
  Result<Path> loadOriginalFont(const Path &originalAssetPath);

  /**
   * @brief Load prefab asset
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for prefab result
   */
  Result<Path> loadOriginalPrefab(const Path &originalAssetPath);

  /**
   * @brief Load original environment
   *
   * @param originalAssetPath Original asset path
   * @return Path to engine asset for environment result
   */
  Result<Path> loadOriginalEnvironment(const Path &originalAssetPath);

private:
  AssetCache mAssetCache;
  Path mAssetsPath;
  bool mOptimize;

  ImageLoader mImageLoader;

  HDRIImporter mHDRIImporter;

  std::unordered_map<String, Path> mAssetCacheMap;
};

} // namespace liquid::editor
