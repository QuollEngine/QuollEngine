#include "liquid/core/Base.h"
#include "AssetRegistry.h"

namespace liquid {

AssetRegistry::~AssetRegistry() {
  for (auto &[_, texture] : mTextures.getAssets()) {
    delete[](texture.data.data);
  }
}

void AssetRegistry::syncWithDeviceRegistry(rhi::ResourceRegistry &registry) {
  LIQUID_PROFILE_EVENT("AssetRegistry::syncWithDeviceRegistry");
  for (auto &[_, texture] : mTextures.getAssets()) {
    if (texture.data.deviceHandle == rhi::TextureHandle::Invalid) {
      rhi::TextureDescription description;

      description.data = texture.data.data;
      description.width = texture.data.width;
      description.layers = texture.data.layers;
      description.height = texture.data.height;
      description.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
      description.type = rhi::TextureType::Standard;
      description.size = texture.size;
      description.format = VK_FORMAT_R8G8B8A8_SRGB;

      texture.data.deviceHandle = registry.setTexture(description);
    }
  }
}

AssetType AssetRegistry::getAssetType(const std::filesystem::path &filePath) {
  LIQUID_PROFILE_EVENT("AssetRegistry::getAssetType");
  for (auto &[_, asset] : mTextures.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::Texture;
    }
  }

  for (auto &[_, asset] : mMaterials.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::Material;
    }
  }

  for (auto &[_, asset] : mMeshes.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::Mesh;
    }
  }

  for (auto &[_, asset] : mSkinnedMeshes.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::SkinnedMesh;
    }
  }

  for (auto &[_, asset] : mSkeletons.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::Skeleton;
    }
  }

  for (auto &[_, asset] : mAnimations.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::Animation;
    }
  }

  for (auto &[_, asset] : mPrefabs.getAssets()) {
    if (asset.path == filePath) {
      return AssetType::Prefab;
    }
  }

  return AssetType::None;
}

} // namespace liquid
