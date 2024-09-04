#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetRevision.h"
#include "HDRIImporter.h"
#include "ImageLoader.h"
#include "UUIDMap.h"

namespace quoll {

class RenderStorage;

} // namespace quoll

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
  static const std::vector<String> TextureExtensions;

  static const std::vector<String> ScriptExtensions;

  static const std::vector<String> AudioExtensions;

  static const std::vector<String> FontExtensions;

  static const std::vector<String> AnimatorExtensions;

  static const std::vector<String> InputMapExtensions;

  static const std::vector<String> PrefabExtensions;

  static const std::vector<String> SceneExtensions;

  static const std::vector<String> EnvironmentExtensions;

public:
  struct SourceInfo {
    Uuid uuid;

    String name;

    AssetType type;

    bool hasContents;
  };

public:
  AssetManager(const Path &assetsPath, const Path &assetsCachePath,
               RenderStorage &renderStorage, bool optimize,
               bool createDefaultObjects);

  Result<Path> importAsset(const Path &source,
                           const Path &targetAssetDirectory);

  inline AssetRegistry &getAssetRegistry() { return mAssetCache.getRegistry(); }

  inline const Path &getAssetsPath() const { return mAssetsPath; }

  Result<void> syncAssets();

  inline AssetCache &getCache() { return mAssetCache; }

  rhi::TextureHandle generatePreview(const Uuid &uuid);

  const SourceInfo &getSourceInfo(const Path &path);

  const std::vector<SourceInfo> &getSourceContentInfos(const Path &path);

  Uuid findRootAssetUuid(const Path &sourceAssetPath);

  Result<void> createDirectory(const Path &assetsPath);

  Result<Path> createLuaScript(const Path &assetPath);

  Result<Path> createAnimator(const Path &assetPath);

  Result<Path> createInputMap(const Path &assetPath);

  Result<UUIDMap> loadSourceIfChanged(const Path &sourceAssetPath);

  static AssetType getAssetTypeFromExtension(const Path &path);

private:
  static String getFileHash(const Path &path);

  static std::optional<Path> createDirectoriesRecursive(const Path &path);

private:
  Path getMetaFilePath(const Path &sourceAssetPath) const;

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

  Result<Path> createMetaFile(const Path &sourceAssetPath, const UUIDMap &uuids,
                              AssetRevision revision);

private:
  Result<UUIDMap> loadSourceAsset(const Path &sourceAssetPath,
                                  const UUIDMap &uuids);

  Result<UUIDMap> loadSourceTexture(const Path &sourceAssetPath,
                                    const UUIDMap &uuids);

  Result<UUIDMap> loadSourceAudio(const Path &sourceAssetPath,
                                  const UUIDMap &uuids);

  Result<UUIDMap> loadSourceScript(const Path &sourceAssetPath,
                                   const UUIDMap &uuids);

  Result<UUIDMap> loadSourceFont(const Path &sourceAssetPath,
                                 const UUIDMap &uuid);

  Result<UUIDMap> loadSourceAnimator(const Path &sourceAssetPath,
                                     const UUIDMap &uuids);

  Result<UUIDMap> loadSourceInputMap(const Path &sourceAssetPath,
                                     const UUIDMap &uuids);

  Result<UUIDMap> loadSourcePrefab(const Path &sourceAssetPath,
                                   const UUIDMap &uuids);

  Result<UUIDMap> loadSourceEnvironment(const Path &sourceAssetPath,
                                        const UUIDMap &uuids);

  Result<UUIDMap> loadSourceScene(const Path &sourceAssetPath,
                                  const UUIDMap &uuids);

private:
  RenderStorage &mRenderStorage;
  AssetCache mAssetCache;
  Path mAssetsPath;
  bool mOptimize;

  ImageLoader mImageLoader;

  HDRIImporter mHDRIImporter;

  std::unordered_map<Path, Uuid> mSourceToRootUuids;
  std::unordered_map<Uuid, Path> mUuidToSources;
  std::unordered_map<Path, SourceInfo> mSourceInfos;
  std::unordered_map<Path, std::vector<SourceInfo>> mSourceContentInfos;
  std::unordered_map<Uuid, rhi::TextureHandle> mPreviews;
};

} // namespace quoll::editor
