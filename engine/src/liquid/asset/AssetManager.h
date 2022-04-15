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
   * @brief Create material from asset
   *
   * Create engine specific material asset
   * from material data
   *
   * @param asset Material asset
   * @return Path to new material asset
   */
  std::filesystem::path
  createMaterialFromAsset(const AssetData<MaterialAsset> &asset);

  /**
   * @brief Load material from file
   *
   * @param filePath Path to asset
   * @return Material asset handle
   */
  MaterialAssetHandle
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
  std::filesystem::path createMeshFromAsset(const AssetData<MeshAsset> &asset);

  /**
   * @brief Load mesh from file
   *
   * @param filePath Path to asset
   * @return Mesh asset handle
   */
  MeshAssetHandle loadMeshFromFile(const std::filesystem::path &filePath);

  /**
   * @brief Create skinned mesh from asset
   *
   * Create engine specific skinned mesh asset
   * from mesh data
   *
   * @param asset Mesh asset
   * @return Path to new mesh asset
   */
  std::filesystem::path
  createSkinnedMeshFromAsset(const AssetData<SkinnedMeshAsset> &asset);

  /**
   * @brief Load skinned mesh from file
   *
   * @param filePath Path to asset
   * @return Skinned mesh asset handle
   */
  SkinnedMeshAssetHandle
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
  std::filesystem::path
  createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset);

  /**
   * @brief Load skeleton from file
   *
   * @param filePath Path to asset
   * @return Skeleton asset handle
   */
  SkeletonAssetHandle
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
  std::filesystem::path
  createAnimationFromAsset(const AssetData<AnimationAsset> &asset);

  /**
   * @brief Load animation from file
   *
   * @param filePath Path to asset
   * @return Animation asset handle
   */
  AnimationAssetHandle
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
  std::filesystem::path
  createPrefabFromAsset(const AssetData<PrefabAsset> &asset);

  /**
   * @brief Load prefab from file
   *
   * @param filePath Path to asset
   * @return Prefab asset handle
   */
  PrefabAssetHandle loadPrefabFromFile(const std::filesystem::path &filePath);

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
