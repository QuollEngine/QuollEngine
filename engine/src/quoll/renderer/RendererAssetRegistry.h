#pragma once

#include "quoll/asset/AssetRef.h"
#include "quoll/text/FontAsset.h"
#include "Material.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "MeshDrawData.h"
#include "TextureAsset.h"

namespace quoll {

class RenderStorage;

class RendererAssetRegistry {
public:
  RendererAssetRegistry(RenderStorage &storage);

  rhi::TextureHandle get(const AssetRef<TextureAsset> &asset);

  Material *get(const AssetRef<MaterialAsset> &asset);

  const MeshDrawData &get(const AssetRef<MeshAsset> &asset);

  rhi::TextureHandle get(const AssetRef<FontAsset> &asset);

private:
  RenderStorage &mStorage;

  std::unordered_map<AssetHandle<TextureAsset>, rhi::TextureHandle> mTextures;
  std::unordered_map<AssetHandle<MaterialAsset>, std::unique_ptr<Material>>
      mMaterials;
  std::unordered_map<AssetHandle<MeshAsset>, MeshDrawData> mMeshBuffers;
  std::unordered_map<AssetHandle<FontAsset>, rhi::TextureHandle> mFontAtlases;
};

} // namespace quoll
