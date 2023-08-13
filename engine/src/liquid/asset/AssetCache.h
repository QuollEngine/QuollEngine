#pragma once

#include "Result.h"
#include "AssetRegistry.h"
#include "AssetFileHeader.h"
#include "AssetMeta.h"

namespace liquid {

class InputBinaryStream;

/**
 * @brief Asset cache
 *
 * Loads and creates engine optimized
 * assets for usage within engine
 * functionality
 */
class AssetCache {
public:
  /**
   * @brief Create asset cache
   *
   * @param assetsPath Assets path
   * @param createDefaultObjects Create default objects
   */
  AssetCache(const Path &assetsPath, bool createDefaultObjects = false);

  /**
   * @brief Create texture from source
   *
   * @param sourcePath Source path
   * @param uuid Asset uui
   * @return Path to new texture asset
   */
  Result<Path> createTextureFromSource(const Path &sourcePath,
                                       const String &uuid);

  /**
   * @brief Create texture from asset
   *
   * Create engine specific texture
   * asset from any texture asset
   *
   * @param asset Texture asset
   * @param uuid Asset uuid
   * @return Path to new texture asset
   */
  Result<Path> createTextureFromAsset(const AssetData<TextureAsset> &asset,
                                      const String &uuid);

  /**
   * @brief Load texture from file
   *
   * @param filePath Path to asset
   * @return Texture asset handle
   */
  Result<TextureAssetHandle> loadTextureFromFile(const Path &filePath);

  /**
   * @brief Create font from source
   *
   * @param sourcePath Source path
   * @param uuid Asset uuid
   * @return Path to new font asset
   */
  Result<Path> createFontFromSource(const Path &sourcePath, const String &uuid);

  /**
   * @brief Load font from file
   *
   * @param filePath Path to asset
   * @return Font asset handle
   */
  Result<FontAssetHandle> loadFontFromFile(const Path &filePath);

  /**
   * @brief Create material from asset
   *
   * Create engine specific material asset
   * from material data
   *
   * @param asset Material asset
   * @param uuid Asset uuid
   * @return Path to new material asset
   */
  Result<Path> createMaterialFromAsset(const AssetData<MaterialAsset> &asset,
                                       const String &uuid);

  /**
   * @brief Load material from file
   *
   * @param filePath Path to asset
   * @return Material asset handle
   */
  Result<MaterialAssetHandle> loadMaterialFromFile(const Path &filePath);

  /**
   * @brief Create mesh from asset
   *
   * Create engine specific mesh asset
   * from mesh data
   *
   * @param asset Mesh asset
   * @param uuid Asset uuid
   * @return Path to new mesh asset
   */
  Result<Path> createMeshFromAsset(const AssetData<MeshAsset> &asset,
                                   const String &uuid);

  /**
   * @brief Load mesh from file
   *
   * @param filePath Path to asset
   * @return Mesh asset handle
   */
  Result<MeshAssetHandle> loadMeshFromFile(const Path &filePath);

  /**
   * @brief Create skinned mesh from asset
   *
   * Create engine specific skinned mesh asset
   * from mesh data
   *
   * @param asset Mesh asset
   * @param uuid Asset uuid
   * @return Path to new mesh asset
   */
  Result<Path>
  createSkinnedMeshFromAsset(const AssetData<SkinnedMeshAsset> &asset,
                             const String &uuid);

  /**
   * @brief Load skinned mesh from file
   *
   * @param filePath Path to asset
   * @return Skinned mesh asset handle
   */
  Result<SkinnedMeshAssetHandle> loadSkinnedMeshFromFile(const Path &filePath);

  /**
   * @brief Create skeleton from asset
   *
   * Create engine specific skeleton asset
   * from mesh data
   *
   * @param asset Skeleton asset
   * @param uuid Asset uuid
   * @return Path to new skeleton asset
   */
  Result<Path> createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset,
                                       const String &uuid);

  /**
   * @brief Load skeleton from file
   *
   * @param filePath Path to asset
   * @return Skeleton asset handle
   */
  Result<SkeletonAssetHandle> loadSkeletonFromFile(const Path &filePath);

  /**
   * @brief Create animation from asset
   *
   * Create engine specific animation asset
   * from animation data
   *
   * @param asset Animation asset
   * @param uuid Existing uuid
   * @return Path to new animation asset
   */
  Result<Path> createAnimationFromAsset(const AssetData<AnimationAsset> &asset,
                                        const String &uuid);

  /**
   * @brief Load animation from file
   *
   * @param filePath Path to asset
   * @return Animation asset handle
   */
  Result<AnimationAssetHandle> loadAnimationFromFile(const Path &filePath);

  /**
   * @brief Create animator from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new animator asset
   */
  Result<Path> createAnimatorFromSource(const Path &sourcePath,
                                        const String &uuid);

  /**
   * @brief Create animator from asset
   *
   * Create engine specific animator asset
   * from animator data
   *
   * @param asset Animator asset
   * @param uuid Existing uuid
   * @return Path to new animator asset
   */
  Result<Path> createAnimatorFromAsset(const AssetData<AnimatorAsset> &asset,
                                       const String &uuid);

  /**
   * @brief Load animator from file
   *
   * @param filePath Path to asset
   * @param handle Existing asset handle
   * @return Animator asset handle
   */
  Result<AnimatorAssetHandle>
  loadAnimatorFromFile(const Path &filePath,
                       AnimatorAssetHandle handle = AnimatorAssetHandle::Null);

  /**
   * @brief Copy audio from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new audio asset
   */
  Result<Path> createAudioFromSource(const Path &sourcePath,
                                     const String &uuid);

  /**
   * @brief Load audio from file
   *
   * @param filePath Path to asset
   * @return Audio asset handle
   */
  Result<AudioAssetHandle> loadAudioFromFile(const Path &filePath);

  /**
   * @brief Create prefab from asset
   *
   * Create engine specific prefab asset
   * from prefab data
   *
   * @param asset Prefab asset
   * @param uuid Asset uuid
   * @return Path to new prefab asset
   */
  Result<Path> createPrefabFromAsset(const AssetData<PrefabAsset> &asset,
                                     const String &uuid);

  /**
   * @brief Load prefab from file
   *
   * @param filePath Path to asset
   * @return Prefab asset handle
   */
  Result<PrefabAssetHandle> loadPrefabFromFile(const Path &filePath);

  /**
   * @brief Create environment from asset
   *
   * Create engine specific environment asset
   * from environment data
   *
   * @param asset Environment asset
   * @param uuid Existing uuid
   * @return Path to new environment asset
   */
  Result<Path>
  createEnvironmentFromAsset(const AssetData<EnvironmentAsset> &asset,
                             const String &uuid);

  /**
   * @brief Load environment from file
   *
   * @param filePath Path to asset
   * @return Environment asset handle
   */
  Result<EnvironmentAssetHandle> loadEnvironmentFromFile(const Path &filePath);

  /**
   * @brief Create Lua script from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new Lua script
   */
  Result<Path> createLuaScriptFromSource(const Path &sourcePath,
                                         const String &uuid);

  /**
   * @brief Load Lua script from file
   *
   * @param filePath Path to asset
   * @param handle Lua script handle
   * @return Lua script handle
   */
  Result<LuaScriptAssetHandle> loadLuaScriptFromFile(
      const Path &filePath,
      LuaScriptAssetHandle handle = LuaScriptAssetHandle::Null);

  /**
   * @brief Get asset registry
   *
   * @return Asset registry
   */
  inline AssetRegistry &getRegistry() { return mRegistry; }

  /**
   * @brief Get assets path
   *
   * @return Assets path
   */
  inline const Path &getAssetsPath() const { return mAssetsPath; }

  /**
   * @brief Preload all assets in assets directory
   *
   * @param renderStorage Render storage
   * @return Preload result
   */
  Result<bool> preloadAssets(RenderStorage &renderStorage);

  /**
   * @brief Load single asset
   *
   * Automatically identifies the asset type
   * and loads it to registry
   *
   * @param path Path to asset
   * @return Load result
   */
  Result<bool> loadAsset(const Path &path);

  /**
   * @brief Get meta from uuid
   *
   * @param uuid Asset uuid
   * @return Asset meta
   */
  AssetMeta getMetaFromUuid(const String &uuid) const;

  /**
   * @brief Create asset metafile
   *
   * Creates file in the same path as
   * the path of the asset
   *
   * @param type Asset type
   * @param name Asset name
   * @param path Full path to asset
   * @return Path to meta file
   */
  Result<Path> createMetaFile(AssetType type, String name, Path path);

private:
  /**
   * @brief Get uuid of asset
   *
   * @tparam TAssetMap Asset map type
   * @param map Asset map
   * @param handle Asset handle
   * @return Uuid of asset
   */
  template <class TAssetMap>
  String getAssetUuid(TAssetMap &map, typename TAssetMap::Handle handle) {
    if (handle != TAssetMap::Handle::Null) {
      return map.getAsset(handle).uuid;
    }

    return String("");
  }

  /**
   * @brief Check asset file for validity
   *
   * Check if the binary stream did not fail,
   * the header has the correct magic word,
   * and the asset type is correct
   *
   * @return Asset file header
   */
  Result<AssetFileHeader> checkAssetFile(InputBinaryStream &file,
                                         const Path &filePath,
                                         AssetType assetType);

  /**
   * @brief Load single asset
   *
   * Automatically identifies the asset type
   * and loads it to registry
   *
   * @param path Path to asset
   * @param updateExisting Update if asset already exists
   * @return Load result
   */
  Result<bool> loadAsset(const Path &path, bool updateExisting);

private:
  /**
   * @brief Load material from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Material asset handle
   */
  Result<MaterialAssetHandle>
  loadMaterialDataFromInputStream(InputBinaryStream &stream,
                                  const Path &filePath,
                                  const AssetFileHeader &header);

  /**
   * @brief Load mesh from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Mesh asset handle
   */
  Result<MeshAssetHandle>
  loadMeshDataFromInputStream(InputBinaryStream &stream, const Path &filePath,
                              const AssetFileHeader &header);

  /**
   * @brief Load skinned mesh from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Skinned mesh asset handle
   */
  Result<SkinnedMeshAssetHandle>
  loadSkinnedMeshDataFromInputStream(InputBinaryStream &stream,
                                     const Path &filePath,
                                     const AssetFileHeader &header);

  /**
   * @brief Load skeleton from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Skeleton asset handle
   */
  Result<SkeletonAssetHandle>
  loadSkeletonDataFromInputStream(InputBinaryStream &stream,
                                  const Path &filePath,
                                  const AssetFileHeader &header);
  /**
   * @brief Load animation from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Animation asset handle
   */
  Result<AnimationAssetHandle>
  loadAnimationDataFromInputStream(InputBinaryStream &stream,
                                   const Path &filePath,
                                   const AssetFileHeader &header);

  /**
   * @brief Load environment from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Environment asset handle
   */
  Result<EnvironmentAssetHandle>
  loadEnvironmentDataFromInputStream(InputBinaryStream &stream,
                                     const Path &filePath,
                                     const AssetFileHeader &header);

  /**
   * @brief Load prefab from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @param header Asset file header
   * @return Prefab asset handle
   */
  Result<PrefabAssetHandle>
  loadPrefabDataFromInputStream(InputBinaryStream &stream, const Path &filePath,
                                const AssetFileHeader &header);

private:
  /**
   * @brief Get asset path from uuid
   *
   * @param uuid Asset uuid
   * @return Path to asset
   */
  Path getPathFromUuid(const String &uuid);

  /**
   * @brief Get or load texture from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded texture
   */
  Result<TextureAssetHandle> getOrLoadTextureFromUuid(const String &uuid);

  /**
   * @brief Get or load material from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded material
   */
  Result<MaterialAssetHandle> getOrLoadMaterialFromUuid(const String &uuid);

  /**
   * @brief Get or load mesh from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded mesh
   */
  Result<MeshAssetHandle> getOrLoadMeshFromUuid(const String &uuid);

  /**
   * @brief Get or load skinned mesh from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded skinned mesh
   */
  Result<SkinnedMeshAssetHandle>
  getOrLoadSkinnedMeshFromUuid(const String &uuid);

  /**
   * @brief Get or load skeleton from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded skeleton
   */
  Result<SkeletonAssetHandle> getOrLoadSkeletonFromUuid(const String &uuid);

  /**
   * @brief Get or load animation from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded animation
   */
  Result<AnimationAssetHandle> getOrLoadAnimationFromUuid(const String &uuid);

  /**
   * @brief Get or load animator from uuid
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded animator
   */
  Result<AnimatorAssetHandle> getOrLoadAnimatorFromUuid(const String &uuid);

private:
  /**
   * @brief Generate asset uuid
   *
   * @return New asset uuid
   */
  String generateUUID();

  /**
   * @brief Create asset path
   *
   * @param uuid Asset uui
   * @return Asset path
   */
  Path createAssetPath(const String &uuid);

private:
  AssetRegistry mRegistry;
  Path mAssetsPath;
};

} // namespace liquid
