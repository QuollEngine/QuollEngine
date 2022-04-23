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

  // Synchronize textures
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

  // Synchronize meshes
  for (auto &[_, mesh] : mMeshes.getAssets()) {
    if (mesh.data.vertexBuffers.empty()) {
      mesh.data.vertexBuffers.resize(mesh.data.geometries.size(),
                                     rhi::BufferHandle::Invalid);
      mesh.data.indexBuffers.resize(mesh.data.geometries.size(),
                                    rhi::BufferHandle::Invalid);
    }

    for (size_t i = 0; i < mesh.data.geometries.size(); ++i) {
      auto &geometry = mesh.data.geometries.at(i);

      {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Vertex;
        description.size = geometry.vertices.size() * sizeof(Vertex);
        description.data = geometry.vertices.data();
        mesh.data.vertexBuffers.at(i) = registry.setBuffer(description);
      }

      if (!geometry.indices.empty()) {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Index;
        description.size = geometry.indices.size() * sizeof(uint32_t);
        description.data = geometry.indices.data();
        mesh.data.indexBuffers.at(i) = registry.setBuffer(description);
      }
    }
  }

  // Synchronize skinned meshes
  for (auto &[_, mesh] : mSkinnedMeshes.getAssets()) {
    if (mesh.data.vertexBuffers.empty()) {
      mesh.data.vertexBuffers.resize(mesh.data.geometries.size(),
                                     rhi::BufferHandle::Invalid);
      mesh.data.indexBuffers.resize(mesh.data.geometries.size(),
                                    rhi::BufferHandle::Invalid);
    }

    for (size_t i = 0; i < mesh.data.geometries.size(); ++i) {
      auto &geometry = mesh.data.geometries.at(i);

      {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Vertex;
        description.size = geometry.vertices.size() * sizeof(SkinnedVertex);
        description.data = geometry.vertices.data();
        mesh.data.vertexBuffers.at(i) = registry.setBuffer(description);
      }

      if (!geometry.indices.empty()) {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Index;
        description.size = geometry.indices.size() * sizeof(uint32_t);
        description.data = geometry.indices.data();
        mesh.data.indexBuffers.at(i) = registry.setBuffer(description);
      }
    }
  }
}

std::pair<AssetType, uint32_t>
AssetRegistry::getAssetByPath(const std::filesystem::path &filePath) {
  LIQUID_PROFILE_EVENT("AssetRegistry::getAssetType");
  for (auto &[handle, asset] : mTextures.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Texture, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mMaterials.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Material, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mMeshes.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Mesh, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mSkinnedMeshes.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::SkinnedMesh, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mSkeletons.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Skeleton, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mAnimations.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Animation, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mPrefabs.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Prefab, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mLuaScripts.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::LuaScript, static_cast<uint32_t>(handle)};
    }
  }

  return {AssetType::None, 0};
}

} // namespace liquid
