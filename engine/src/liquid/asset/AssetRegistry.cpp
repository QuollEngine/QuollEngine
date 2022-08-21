#include "liquid/core/Base.h"
#include "liquid/renderer/MaterialPBR.h"
#include "AssetRegistry.h"

#include "DefaultObjects.h"

namespace liquid {

AssetRegistry::~AssetRegistry() {
  for (auto &[_, texture] : mTextures.getAssets()) {
    delete[](texture.data.data);
  }
}

void AssetRegistry::createDefaultObjects() {
  auto mesh = default_objects::createCube();
  mDefaultObjects.cube = mMeshes.addAsset(mesh);
  mDefaultObjects.defaultMaterial =
      mMaterials.addAsset(default_objects::createDefaultMaterial());
  mDefaultObjects.defaultFont =
      mFonts.addAsset(default_objects::createDefaultFont());
}

void AssetRegistry::syncWithDevice(rhi::RenderDevice *device) {
  LIQUID_PROFILE_EVENT("AssetRegistry::syncWithDevice");

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
      description.type = texture.data.type == TextureAssetType::Cubemap
                             ? rhi::TextureType::Cubemap
                             : rhi::TextureType::Standard;
      description.size = texture.size;
      description.format = texture.data.format;

      texture.data.deviceHandle = device->createTexture(description);
    }
  }

  // Synchronize fonts
  for (auto &[_, font] : mFonts.getAssets()) {
    if (font.data.deviceHandle == rhi::TextureHandle::Invalid) {
      rhi::TextureDescription description{};
      description.data = font.data.atlas.data();
      description.size = font.size;
      description.width = font.data.atlasDimensions.x;
      description.height = font.data.atlasDimensions.y;
      description.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
      description.format = VK_FORMAT_R8G8B8A8_SRGB;

      font.data.deviceHandle = device->createTexture(description);
    }
  }

  // Synchronize materials
  auto getTextureFromRegistry = [this](TextureAssetHandle handle) {
    if (handle != TextureAssetHandle::Invalid) {
      return mTextures.getAsset(handle).data.deviceHandle;
    }

    return rhi::TextureHandle::Invalid;
  };

  for (auto &[_, asset] : mMaterials.getAssets()) {
    auto &material = asset.data;
    if (!material.deviceHandle) {
      liquid::MaterialPBR::Properties properties{};

      properties.baseColorFactor = material.baseColorFactor;
      properties.baseColorTexture =
          getTextureFromRegistry(material.baseColorTexture);
      properties.baseColorTextureCoord = material.baseColorTextureCoord;

      properties.metallicFactor = material.metallicFactor;
      properties.metallicRoughnessTexture =
          getTextureFromRegistry(material.metallicRoughnessTexture);
      properties.metallicRoughnessTextureCoord =
          material.metallicRoughnessTextureCoord;

      properties.normalScale = material.normalScale;
      properties.normalTexture = getTextureFromRegistry(material.normalTexture);

      properties.normalTextureCoord = material.normalTextureCoord;

      properties.occlusionStrength = material.occlusionStrength;
      properties.occlusionTexture =
          getTextureFromRegistry(material.occlusionTexture);
      properties.occlusionTextureCoord = material.occlusionTextureCoord;

      properties.emissiveFactor = material.emissiveFactor;
      properties.emissiveTexture =
          getTextureFromRegistry(material.emissiveTexture);
      properties.emissiveTextureCoord = material.emissiveTextureCoord;

      material.deviceHandle.reset(new MaterialPBR(properties, device));
    }
  }

  // Synchronize meshes
  for (auto &[_, mesh] : mMeshes.getAssets()) {
    if (mesh.data.vertexBuffers.empty()) {
      mesh.data.vertexBuffers.resize(mesh.data.geometries.size());
      mesh.data.indexBuffers.resize(mesh.data.geometries.size());
      mesh.data.materials.resize(mesh.data.geometries.size(), nullptr);
    }

    for (size_t i = 0; i < mesh.data.geometries.size(); ++i) {
      auto &geometry = mesh.data.geometries.at(i);

      {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Vertex;
        description.size = geometry.vertices.size() * sizeof(Vertex);
        description.data = geometry.vertices.data();
        mesh.data.vertexBuffers.at(i) = device->createBuffer(description);
      }

      if (!geometry.indices.empty()) {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Index;
        description.size = geometry.indices.size() * sizeof(uint32_t);
        description.data = geometry.indices.data();
        mesh.data.indexBuffers.at(i) = device->createBuffer(description);
      }

      auto material = geometry.material != MaterialAssetHandle::Invalid
                          ? geometry.material
                          : mDefaultObjects.defaultMaterial;
      mesh.data.materials.at(i) =
          mMaterials.getAsset(material).data.deviceHandle;
    }
  }

  // Synchronize skinned meshes
  for (auto &[_, mesh] : mSkinnedMeshes.getAssets()) {
    if (mesh.data.vertexBuffers.empty()) {
      mesh.data.vertexBuffers.resize(mesh.data.geometries.size());
      mesh.data.indexBuffers.resize(mesh.data.geometries.size());
      mesh.data.materials.resize(mesh.data.geometries.size(), nullptr);
    }

    for (size_t i = 0; i < mesh.data.geometries.size(); ++i) {
      auto &geometry = mesh.data.geometries.at(i);

      {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Vertex;
        description.size = geometry.vertices.size() * sizeof(SkinnedVertex);
        description.data = geometry.vertices.data();
        mesh.data.vertexBuffers.at(i) = device->createBuffer(description);
      }

      if (!geometry.indices.empty()) {
        rhi::BufferDescription description;
        description.type = rhi::BufferType::Index;
        description.size = geometry.indices.size() * sizeof(uint32_t);
        description.data = geometry.indices.data();
        mesh.data.indexBuffers.at(i) = device->createBuffer(description);
      }

      auto material = geometry.material != MaterialAssetHandle::Invalid
                          ? geometry.material
                          : mDefaultObjects.defaultMaterial;
      mesh.data.materials.at(i) =
          mMaterials.getAsset(material).data.deviceHandle;
    }
  }
}

std::pair<AssetType, uint32_t>
AssetRegistry::getAssetByPath(const Path &filePath) {
  LIQUID_PROFILE_EVENT("AssetRegistry::getAssetType");
  for (auto &[handle, asset] : mTextures.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Texture, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mFonts.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Font, static_cast<uint32_t>(handle)};
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

  for (auto &[handle, asset] : mAudios.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Audio, static_cast<uint32_t>(handle)};
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
