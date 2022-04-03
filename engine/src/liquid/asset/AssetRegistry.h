#pragma once

#include "Asset.h"
#include "AssetMap.h"

#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "MeshAsset.h"

#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

class AssetRegistry {
  using TextureMap = AssetMap<TextureAssetHandle, TextureAsset>;
  using MaterialMap = AssetMap<MaterialAssetHandle, MaterialAsset>;
  using MeshMap = AssetMap<MeshAssetHandle, MeshAsset<Vertex>>;
  using SkinnedMeshMap =
      AssetMap<SkinnedMeshAssetHandle, MeshAsset<SkinnedVertex>>;

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

private:
  TextureMap mTextures;
  MaterialMap mMaterials;
  MeshMap mMeshes;
  SkinnedMeshMap mSkinnedMeshes;
};

} // namespace liquid
