#pragma once

#include "Result.h"
#include "AssetRegistry.h"
#include "AssetFileHeader.h"
#include "AssetMeta.h"

namespace quoll {

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
   * @param uuid Asset uuid
   * @return Path to new texture asset
   */
  Result<Path> createTextureFromSource(const Path &sourcePath,
                                       const Uuid &uuid);

  /**
   * @brief Create texture from asset
   *
   * Create engine specific texture
   * asset from any texture asset
   *
   * @param asset Texture asset
   * @return Path to new texture asset
   */
  Result<Path> createTextureFromAsset(const AssetData<TextureAsset> &asset);

  /**
   * @brief Load texture from file
   *
   * @param uuid Texture uuid
   * @return Texture asset handle
   */
  Result<TextureAssetHandle> loadTexture(const Uuid &uuid);

  /**
   * @brief Create font from source
   *
   * @param sourcePath Source path
   * @param uuid Asset uuid
   * @return Path to new font asset
   */
  Result<Path> createFontFromSource(const Path &sourcePath, const Uuid &uuid);

  /**
   * @brief Load font from file
   *
   * @param uuid Font uuid
   * @return Font asset handle
   */
  Result<FontAssetHandle> loadFont(const Uuid &uuid);

  /**
   * @brief Create material from asset
   *
   * Create engine specific material asset
   * from material data
   *
   * @param asset Material asset
   * @return Path to new material asset
   */
  Result<Path> createMaterialFromAsset(const AssetData<MaterialAsset> &asset);

  /**
   * @brief Load material from file
   *
   * @param uuid Material uuid
   * @return Material asset handle
   */
  Result<MaterialAssetHandle> loadMaterial(const Uuid &uuid);

  /**
   * @brief Create mesh from asset
   *
   * Create engine specific mesh asset
   * from mesh data
   *
   * @param asset Mesh asset
   * @return Path to new mesh asset
   */
  Result<Path> createMeshFromAsset(const AssetData<MeshAsset> &asset);

  /**
   * @brief Load mesh from file
   *
   * @param uuid Mesh uuid
   * @return Mesh asset handle
   */
  Result<MeshAssetHandle> loadMesh(const Uuid &uuid);

  /**
   * @brief Create skeleton from asset
   *
   * Create engine specific skeleton asset
   * from mesh data
   *
   * @param asset Skeleton asset
   * @return Path to new skeleton asset
   */
  Result<Path> createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset);

  /**
   * @brief Load skeleton
   *
   * @param uuid Skeleton uuid
   * @return Skeleton asset handle
   */
  Result<SkeletonAssetHandle> loadSkeleton(const Uuid &uuid);

  /**
   * @brief Create animation from asset
   *
   * Create engine specific animation asset
   * from animation data
   *
   * @param asset Animation asset
   * @return Path to new animation asset
   */
  Result<Path> createAnimationFromAsset(const AssetData<AnimationAsset> &asset);

  /**
   * @brief Load animation
   *
   * @param uuid Animation uuid
   * @return Animation asset handle
   */
  Result<AnimationAssetHandle> loadAnimation(const Uuid &uuid);

  /**
   * @brief Create animator from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new animator asset
   */
  Result<Path> createAnimatorFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  /**
   * @brief Create animator from asset
   *
   * Create engine specific animator asset
   * from animator data
   *
   * @param asset Animator asset
   * @return Path to new animator asset
   */
  Result<Path> createAnimatorFromAsset(const AssetData<AnimatorAsset> &asset);

  /**
   * @brief Load animator
   *
   * @param uuid Animator uuid
   * @return Animator asset handle
   */
  Result<AnimatorAssetHandle> loadAnimator(const Uuid &uuid);

  /**
   * @brief Copy audio from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new audio asset
   */
  Result<Path> createAudioFromSource(const Path &sourcePath, const Uuid &uuid);

  /**
   * @brief Load audio
   *
   * @param uuid Audio uuid
   * @return Audio asset handle
   */
  Result<AudioAssetHandle> loadAudio(const Uuid &uuid);

  /**
   * @brief Create prefab from asset
   *
   * Create engine specific prefab asset
   * from prefab data
   *
   * @param asset Prefab asset
   * @return Path to new prefab asset
   */
  Result<Path> createPrefabFromAsset(const AssetData<PrefabAsset> &asset);

  /**
   * @brief Load prefab
   *
   * @param uuid Prefab uuid
   * @return Prefab asset handle
   */
  Result<PrefabAssetHandle> loadPrefab(const Uuid &uuid);

  /**
   * @brief Create environment from asset
   *
   * Create engine specific environment asset
   * from environment data
   *
   * @param asset Environment asset
   * @return Path to new environment asset
   */
  Result<Path>
  createEnvironmentFromAsset(const AssetData<EnvironmentAsset> &asset);

  /**
   * @brief Load environment from file
   *
   * @param uuid Environment uuid
   * @return Environment asset handle
   */
  Result<EnvironmentAssetHandle> loadEnvironment(const Uuid &uuid);

  /**
   * @brief Create Lua script from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new Lua script
   */
  Result<Path> createLuaScriptFromSource(const Path &sourcePath,
                                         const Uuid &uuid);

  /**
   * @brief Load Lua script
   *
   * @param uuid Lua script uuid
   * @return Lua script handle
   */
  Result<LuaScriptAssetHandle> loadLuaScript(const Uuid &uuid);

  /**
   * @brief Create scene from source
   *
   * @param sourcePath Source path
   * @param uuid Existing uuid
   * @return Path to new Scene
   */
  Result<Path> createSceneFromSource(const Path &sourcePath, const Uuid &uuid);

  /**
   * @brief Load scene
   *
   * @param uuid Scene uuid
   * @return Scene asset handle
   */
  Result<SceneAssetHandle> loadScene(const Uuid &uuid);

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
   * @brief Get meta from uuid
   *
   * @param uuid Asset uuid
   * @return Asset meta
   */
  AssetMeta getAssetMeta(const Uuid &uuid) const;

  /**
   * @brief Get asset path from uuid
   *
   * @param uuid Asset uuid
   * @return Path to asset
   */
  Path getPathFromUuid(const Uuid &uuid);

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
  Uuid getAssetUuid(TAssetMap &map, typename TAssetMap::Handle handle) {
    if (handle != TAssetMap::Handle::Null) {
      return map.getAsset(handle).uuid;
    }

    return Uuid{};
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
  Result<Path> createAssetMeta(AssetType type, String name, Path path);

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
   * @brief Get or load texture
   *
   * @param uuid Asset uuid
   * @return Existing or newly loaded texture
   */
  Result<TextureAssetHandle> getOrLoadTexture(const Uuid &uuid);

  /**
   * @brief Get or load material
   *
   * @param uuid Material uuid
   * @return Existing or newly loaded material
   */
  Result<MaterialAssetHandle> getOrLoadMaterial(const Uuid &uuid);

  /**
   * @brief Get or load mesh
   *
   * @param uuid Mesh uuid
   * @return Existing or newly loaded mesh
   */
  Result<MeshAssetHandle> getOrLoadMesh(const Uuid &uuid);

  /**
   * @brief Get or load skeleton
   *
   * @param uuid Skeleton uuid
   * @return Existing or newly loaded skeleton
   */
  Result<SkeletonAssetHandle> getOrLoadSkeleton(const Uuid &uuid);

  /**
   * @brief Get or load animation
   *
   * @param uuid Animation uuid
   * @return Existing or newly loaded animation
   */
  Result<AnimationAssetHandle> getOrLoadAnimation(const Uuid &uuid);

  /**
   * @brief Get or load aniamtor from uuid
   *
   * @param uuid Animator uuid
   * @return Existing or newly loaded animator
   */
  Result<AnimatorAssetHandle> getOrLoadAnimator(const Uuid &uuid);

private:
  AssetRegistry mRegistry;
  Path mAssetsPath;
};

} // namespace quoll
