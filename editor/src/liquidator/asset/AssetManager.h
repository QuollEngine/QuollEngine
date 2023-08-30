#pragma once

#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetRevision.h"
#include "ImageLoader.h"
#include "HDRIImporter.h"
#include "UUIDMap.h"

namespace quoll::editor {

/**
 * @brief Manage all the assets in the editor
 *
 * - Import source assets by creating engine assets
 * - Validate assets for changes
 * - Preload assets
 * - Creates assets (e.g script files)
 *
 * Nomeclature:
 *
 * assets directory - Directory of source assets and meta files
 * cache directory - Directory of engine specific assets
 * source asset - Source assets (stored in assets directory)
 * engine asset - Transformed assets that
 *   can be used within the engine (stored in cache directory)
 * meta file - Stores hashes of both source and
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
   * @brief Supported font extensions
   */
  static const std::vector<String> AnimatorExtensions;

  /**
   * @brief Supported prefab extensions
   */
  static const std::vector<String> PrefabExtensions;

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
   * @param renderStorage Render storage
   * @param optimize Optimize assets
   * @param createDefaultObjects Create default objects
   */
  AssetManager(const Path &assetsPath, const Path &assetsCachePath,
               RenderStorage &renderStorage, bool optimize,
               bool createDefaultObjects);

  /**
   * @brief Import asset
   *
   * @param source Path to source asset
   * @param targetAssetDirectory Target directory
   * @return Path to imported source asset
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
   * @brief Get asset cache
   *
   * @return Asset cache
   */
  inline const AssetCache &getCache() const { return mAssetCache; }

  /**
   * @brief Generate preview
   *
   * @param path Source asset path
   * @param renderStorage Render storage
   */
  void generatePreview(const Path &path, RenderStorage &renderStorage);

  /**
   * @brief Find root asset uuid
   *
   * @param sourceAssetPath Source asset path
   * @return Found engine asset root uuid
   */
  Uuid findRootAssetUuid(const Path &sourceAssetPath);

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
   * @brief Create animator in assets
   *
   * @param assetPath Path to assets
   * @return Result
   */
  Result<Path> createAnimator(const Path &assetPath);

  /**
   * @brief Reload assets
   *
   * @return Result
   */
  Result<bool> reloadAssets();

  /**
   * @brief Validate and preload assets
   *
   * @param renderStorage Render storage
   * @return Result
   */
  Result<bool> validateAndPreloadAssets(RenderStorage &renderStorage);

  /**
   * @brief Load source asset if files have changed
   *
   * @param sourceAssetPath Source asset path
   * @return Uuids of the asset
   */
  Result<UUIDMap> loadSourceIfChanged(const Path &sourceAssetPath);

  /**
   * @brief Get render storage
   *
   * @return Render storage
   */
  inline RenderStorage &getRenderStorage() { return mRenderStorage; }

  /**
   * @brief Get asset type from path extension
   *
   * @param path Path to file
   * @return Asset type
   */
  static AssetType getAssetTypeFromExtension(const Path &path);

private:
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
   * @brief Get meta file path from source asset
   *
   * @param sourceAssetPath Source asset path
   * @return Meta file path
   */
  Path getMetaFilePath(const Path &sourceAssetPath) const;

  /**
   * @brief Get uuids from meta
   *
   * @param sourceAssetPath Source asset path
   * @return Uuid map
   */
  UUIDMap getUuidsFromMeta(const Path &sourceAssetPath) const;

  /**
   * @brief Check if asset is changed
   *
   * A changed asset means the following:
   *
   * - Engine file does not exist for the asset
   * - Meta file does not exist for the asset
   * - Asset type has a new revision
   * - Hashes mismatch
   *
   * @param path Source asset path
   * @retval true Asset is changed
   * @retval false Asset is not changed
   */
  bool isAssetChanged(const Path &sourceAssetPath) const;

  /**
   * @brief Create meta file
   *
   * @param sourceAssetPath Source asset path
   * @param uuid Uuid map
   * @param revision Asset revision
   * @return Path to newly created meta file
   */
  Result<Path> createMetaFile(const Path &sourceAssetPath, const UUIDMap &uuids,
                              AssetRevision revision);

private:
  /**
   * @brief Load source asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuids of all loaded assets
   */
  Result<UUIDMap> loadSourceAsset(const Path &sourceAssetPath,
                                  const UUIDMap &uuids);

  /**
   * @brief Load texture asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceTexture(const Path &sourceAssetPath,
                                    const UUIDMap &uuids);

  /**
   * @brief Load audio asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceAudio(const Path &sourceAssetPath,
                                  const UUIDMap &uuids);

  /**
   * @brief Load script asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceScript(const Path &sourceAssetPath,
                                   const UUIDMap &uuids);

  /**
   * @brief Load font asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceFont(const Path &sourceAssetPath,
                                 const UUIDMap &uuid);

  /**
   * @brief Load animator asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceAnimator(const Path &sourceAssetPath,
                                     const UUIDMap &uuid);

  /**
   * @brief Load prefab asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourcePrefab(const Path &sourceAssetPath,
                                   const UUIDMap &uuids);

  /**
   * @brief Load source environment
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceEnvironment(const Path &sourceAssetPath,
                                        const UUIDMap &uuids);

  /**
   * @brief Load scene asset
   *
   * @param sourceAssetPath Source asset path
   * @param uuids Uuid map
   * @return Uuid map
   */
  Result<UUIDMap> loadSourceScene(const Path &sourceAssetPath,
                                  const UUIDMap &uuids);

private:
  RenderStorage &mRenderStorage;
  AssetCache mAssetCache;
  Path mAssetsPath;
  bool mOptimize;

  ImageLoader mImageLoader;

  HDRIImporter mHDRIImporter;

  std::unordered_map<String, Uuid> mAssetCacheMap;
};

} // namespace quoll::editor
