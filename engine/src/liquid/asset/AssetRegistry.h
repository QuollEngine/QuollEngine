#pragma once

#include "Asset.h"
#include "AssetMap.h"

#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "MeshAsset.h"
#include "SkeletonAsset.h"
#include "AnimationAsset.h"
#include "PrefabAsset.h"

#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

class AssetRegistry {
  using TextureMap = AssetMap<TextureAssetHandle, TextureAsset>;
  using MaterialMap = AssetMap<MaterialAssetHandle, MaterialAsset>;
  using MeshMap = AssetMap<MeshAssetHandle, MeshAsset>;
  using SkinnedMeshMap = AssetMap<SkinnedMeshAssetHandle, SkinnedMeshAsset>;
  using SkeletonMap = AssetMap<SkeletonAssetHandle, SkeletonAsset>;
  using AnimationMap = AssetMap<AnimationAssetHandle, AnimationAsset>;
  using PrefabMap = AssetMap<PrefabAssetHandle, PrefabAsset>;

public:
  AssetRegistry() = default;

  /**
   * @brief Destroy registry
   */
  ~AssetRegistry();

  AssetRegistry(const AssetRegistry &) = delete;
  AssetRegistry &operator=(const AssetRegistry &) = delete;
  AssetRegistry(AssetRegistry &&) = delete;
  AssetRegistry &operator=(AssetRegistry &&) = delete;

  /**
   * @brief Synchronize assets with device registry
   *
   * @param registry Device registry
   */
  void syncWithDeviceRegistry(rhi::ResourceRegistry &registry);

  /**
   * @brief Get textures
   *
   * @return Texture asset map
   */
  inline TextureMap &getTextures() { return mTextures; }

  /**
   * @brief Get materials
   *
   * @return Material asset map
   */
  inline MaterialMap &getMaterials() { return mMaterials; }

  /**
   * @brief Get meshes
   *
   * @return Mesh asset map
   */
  inline MeshMap &getMeshes() { return mMeshes; }

  /**
   * @brief Get skinned meshes
   *
   * @return Skinned mesh asset map
   */
  inline SkinnedMeshMap &getSkinnedMeshes() { return mSkinnedMeshes; }

  /**
   * @brief Get skeletons
   *
   * @return Skeleton asset map
   */
  inline SkeletonMap &getSkeletons() { return mSkeletons; }

  /**
   * @brief Get animations
   *
   * @return Animation asset map
   */
  inline AnimationMap &getAnimations() { return mAnimations; }

  /**
   * @brief Get prefabs
   *
   * @return Prefabs asset map
   */
  inline PrefabMap &getPrefabs() { return mPrefabs; }

  /**
   * @brief Get asset located at path
   *
   * @param filePath Path to asset
   * @retval {AssetType::None, 0} Asset does not exist
   * @return Asset type and ID
   */
  std::pair<AssetType, uint32_t>
  getAssetByPath(const std::filesystem::path &filePath);

private:
  TextureMap mTextures;
  MaterialMap mMaterials;
  MeshMap mMeshes;
  SkinnedMeshMap mSkinnedMeshes;
  SkeletonMap mSkeletons;
  AnimationMap mAnimations;
  PrefabMap mPrefabs;
};

} // namespace liquid
