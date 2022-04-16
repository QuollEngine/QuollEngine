#pragma once

#include "Result.h"
#include "AssetRegistry.h"

namespace liquid {

class InputBinaryStream;

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
  Result<std::filesystem::path>
  createTextureFromAsset(const AssetData<TextureAsset> &asset);

  /**
   * @brief Load texture from file
   *
   * @param filePath Path to asset
   * @return Texture asset handle
   */
  Result<TextureAssetHandle>
  loadTextureFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create material from asset
   *
   * Create engine specific material asset
   * from material data
   *
   * @param asset Material asset
   * @return Path to new material asset
   */
  Result<std::filesystem::path>
  createMaterialFromAsset(const AssetData<MaterialAsset> &asset);

  /**
   * @brief Load material from file
   *
   * @param filePath Path to asset
   * @return Material asset handle
   */
  Result<MaterialAssetHandle>
  loadMaterialFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create mesh from asset
   *
   * Create engine specific mesh asset
   * from mesh data
   *
   * @param asset Mesh asset
   * @return Path to new mesh asset
   */
  Result<std::filesystem::path>
  createMeshFromAsset(const AssetData<MeshAsset> &asset);

  /**
   * @brief Load mesh from file
   *
   * @param filePath Path to asset
   * @return Mesh asset handle
   */
  Result<MeshAssetHandle>
  loadMeshFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create skinned mesh from asset
   *
   * Create engine specific skinned mesh asset
   * from mesh data
   *
   * @param asset Mesh asset
   * @return Path to new mesh asset
   */
  Result<std::filesystem::path>
  createSkinnedMeshFromAsset(const AssetData<SkinnedMeshAsset> &asset);

  /**
   * @brief Load skinned mesh from file
   *
   * @param filePath Path to asset
   * @return Skinned mesh asset handle
   */
  Result<SkinnedMeshAssetHandle>
  loadSkinnedMeshFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create skeleton from asset
   *
   * Create engine specific skeleton asset
   * from mesh data
   *
   * @param asset Skeleton asset
   * @return Path to new skeleton asset
   */
  Result<std::filesystem::path>
  createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset);

  /**
   * @brief Load skeleton from file
   *
   * @param filePath Path to asset
   * @return Skeleton asset handle
   */
  Result<SkeletonAssetHandle>
  loadSkeletonFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create animation from asset
   *
   * Create engine specific animation asset
   * from animation data
   *
   * @param asset Animation asset
   * @return Path to new animation asset
   */
  Result<std::filesystem::path>
  createAnimationFromAsset(const AssetData<AnimationAsset> &asset);

  /**
   * @brief Load animation from file
   *
   * @param filePath Path to asset
   * @return Animation asset handle
   */
  Result<AnimationAssetHandle>
  loadAnimationFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create prefab from asset
   *
   * Create engine specific prefab asset
   * from prefab data
   *
   * @param asset Prefab asset
   * @return Path to new prefab asset
   */
  Result<std::filesystem::path>
  createPrefabFromAsset(const AssetData<PrefabAsset> &asset);

  /**
   * @brief Load prefab from file
   *
   * @param filePath Path to asset
   * @return Prefab asset handle
   */
  Result<PrefabAssetHandle>
  loadPrefabFromFile(const std::filesystem::path &filePath);

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
  inline const std::filesystem::path &getAssetsPath() const {
    return mAssetsPath;
  }

  /**
   * @brief Preload all assets in assets directory
   */
  void preloadAssets();

private:
  /**
   * @brief Get relative path of the asset
   *
   * @tparam TAssetMap Asset map type
   * @param map Asset map
   * @param handle Asset handle
   * @return Relative path of asset
   */
  template <class TAssetMap>
  String getAssetRelativePath(TAssetMap &map,
                              typename TAssetMap::Handle handle) {
    if (handle != TAssetMap::Handle::Invalid) {
      auto &texture = map.getAsset(handle);
      auto path = std::filesystem::relative(texture.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');
      return path;
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
   * @return Result
   */
  Result<bool> checkAssetFile(InputBinaryStream &file,
                              const std::filesystem::path &filePath,
                              AssetType assetType);

private:
  /**
   * @brief Load material from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @return Material asset handle
   */
  Result<MaterialAssetHandle>
  loadMaterialDataFromInputStream(InputBinaryStream &stream,
                                  const std::filesystem::path &filePath);

  /**
   * @brief Load mesh from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @return Mesh asset handle
   */
  Result<MeshAssetHandle>
  loadMeshDataFromInputStream(InputBinaryStream &stream,
                              const std::filesystem::path &filePath);

  /**
   * @brief Load skinned mesh from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @return Skinned mesh asset handle
   */
  Result<SkinnedMeshAssetHandle>
  loadSkinnedMeshDataFromInputStream(InputBinaryStream &stream,
                                     const std::filesystem::path &filePath);

  /**
   * @brief Load skeleton from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @return Skeleton asset handle
   */
  Result<SkeletonAssetHandle>
  loadSkeletonDataFromInputStream(InputBinaryStream &stream,
                                  const std::filesystem::path &filePath);
  /**
   * @brief Load animation from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @return Animation asset handle
   */
  Result<AnimationAssetHandle>
  loadAnimationDataFromInputStream(InputBinaryStream &stream,
                                   const std::filesystem::path &filePath);

  /**
   * @brief Load prefab from input stream
   *
   * @param stream Input stream
   * @param filePath Path to asset
   * @return Prefab asset handle
   */
  Result<PrefabAssetHandle>
  loadPrefabDataFromInputStream(InputBinaryStream &stream,
                                const std::filesystem::path &filePath);

private:
  /**
   * @brief Get or load texture from path
   *
   * @param relativePath Path to texture
   * @return Existing or newly loaded texture
   */
  Result<TextureAssetHandle>
  getOrLoadTextureFromPath(const String &relativePath);

  /**
   * @brief Get or load material from path
   *
   * @param relativePath Path to material
   * @return Existing or newly loaded material
   */
  Result<MaterialAssetHandle>
  getOrLoadMaterialFromPath(const String &relativePath);

  /**
   * @brief Get or load mesh from path
   *
   * @param relativePath Path to mesh
   * @return Existing or newly loaded mesh
   */
  Result<MeshAssetHandle> getOrLoadMeshFromPath(const String &relativePath);

  /**
   * @brief Get or load skinned mesh from path
   *
   * @param relativePath Path to skinned mesh
   * @return Existing or newly loaded skinned mesh
   */
  Result<SkinnedMeshAssetHandle>
  getOrLoadSkinnedMeshFromPath(const String &relativePath);

  /**
   * @brief Get or load skeleton from path
   *
   * @param relativePath Path to skeleton
   * @return Existing or newly loaded skeleton
   */
  Result<SkeletonAssetHandle>
  getOrLoadSkeletonFromPath(const String &relativePath);

  /**
   * @brief Get or load animation from path
   *
   * @param relativePath Path to animation
   * @return Existing or newly loaded animation
   */
  Result<AnimationAssetHandle>
  getOrLoadAnimationFromPath(const String &relativePath);

private:
  AssetRegistry mRegistry;
  std::filesystem::path mAssetsPath;
};

} // namespace liquid
