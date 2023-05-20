#include "liquid/core/Base.h"
#include "liquid/renderer/MaterialPBR.h"
#include "liquid/renderer/TextureUtils.h"

#include "AssetRegistry.h"
#include "DefaultObjects.h"

namespace liquid {

void AssetRegistry::createDefaultObjects() {
  auto mesh = default_objects::createCube();
  mDefaultObjects.cube = mMeshes.addAsset(mesh);
  mDefaultObjects.defaultMaterial =
      mMaterials.addAsset(default_objects::createDefaultMaterial());
  mDefaultObjects.defaultFont =
      mFonts.addAsset(default_objects::createDefaultFont());
}

void AssetRegistry::syncWithDevice(RenderStorage &renderStorage) {
  LIQUID_PROFILE_EVENT("AssetRegistry::syncWithDevice");

  // Synchronize textures
  for (auto &[_, texture] : mTextures.getAssets()) {
    if (texture.data.deviceHandle == rhi::TextureHandle::Invalid) {
      rhi::TextureDescription description{};
      description.width = texture.data.width;
      description.levels = static_cast<uint32_t>(texture.data.levels.size());
      description.layers = texture.data.layers;
      description.height = texture.data.height;
      description.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
      description.type = texture.data.type == TextureAssetType::Cubemap
                             ? rhi::TextureType::Cubemap
                             : rhi::TextureType::Standard;
      description.format = texture.data.format;
      description.debugName = texture.name;

      texture.data.deviceHandle = renderStorage.createTexture(description);
      TextureUtils::copyDataToTexture(
          renderStorage.getDevice(), texture.data.data.data(),
          texture.data.deviceHandle, rhi::ImageLayout::ShaderReadOnlyOptimal,
          texture.data.layers, texture.data.levels);
    }
  }

  // Synchronize fonts
  for (auto &[_, font] : mFonts.getAssets()) {
    if (font.data.deviceHandle == rhi::TextureHandle::Invalid) {
      rhi::TextureDescription description{};
      description.width = font.data.atlasDimensions.x;
      description.height = font.data.atlasDimensions.y;
      description.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
      description.format = rhi::Format::Rgba8Srgb;
      description.debugName = font.name;

      font.data.deviceHandle = renderStorage.createTexture(description);
      TextureUtils::copyDataToTexture(
          renderStorage.getDevice(), font.data.atlas.data(),
          font.data.deviceHandle, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
          {{0, font.size, font.data.atlasDimensions.x,
            font.data.atlasDimensions.y}});
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
      properties.roughnessFactor = material.roughnessFactor;

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

      material.deviceHandle.reset(new MaterialPBR(properties, renderStorage));
    }
  }

  // Synchronize meshes
  for (auto &[_, mesh] : mMeshes.getAssets()) {
    if (rhi::isHandleValid(mesh.data.vertexBuffer.getHandle())) {
      continue;
    }

    size_t vbSize = 0;
    size_t ibSize = 0;
    for (auto &g : mesh.data.geometries) {
      vbSize += g.vertices.size() * sizeof(Vertex);
      ibSize += g.indices.size() * sizeof(uint32_t);
    }

    {
      rhi::BufferDescription description;
      description.usage = rhi::BufferUsage::Vertex;
      description.size = vbSize;
      description.data = nullptr;
      description.debugName = mesh.name + " vertex";
      mesh.data.vertexBuffer = renderStorage.createBuffer(description);

      auto *data = static_cast<Vertex *>(mesh.data.vertexBuffer.map());

      size_t offset = 0;
      for (auto &g : mesh.data.geometries) {
        memcpy(data + offset, g.vertices.data(),
               g.vertices.size() * sizeof(Vertex));
        offset += g.vertices.size();
      }

      mesh.data.vertexBuffer.unmap();
    }

    {
      rhi::BufferDescription description;
      description.usage = rhi::BufferUsage::Index;
      description.size = ibSize;
      description.data = nullptr;
      description.debugName = mesh.name + " index";

      mesh.data.indexBuffer = renderStorage.createBuffer(description);

      auto *data = static_cast<uint32_t *>(mesh.data.indexBuffer.map());
      size_t offset = 0;
      for (auto &g : mesh.data.geometries) {
        memcpy(data + offset, g.indices.data(),
               g.indices.size() * sizeof(uint32_t));
        offset += g.indices.size();
      }

      mesh.data.indexBuffer.unmap();
    }

    mesh.data.materials.resize(mesh.data.geometries.size(), nullptr);
    for (size_t i = 0; i < mesh.data.geometries.size(); ++i) {
      auto &geometry = mesh.data.geometries.at(i);
      auto material = geometry.material != MaterialAssetHandle::Invalid
                          ? geometry.material
                          : mDefaultObjects.defaultMaterial;

      mesh.data.materials.at(i) =
          mMaterials.getAsset(material).data.deviceHandle;
    }
  }

  // Synchronize skinned meshes
  for (auto &[_, mesh] : mSkinnedMeshes.getAssets()) {
    if (rhi::isHandleValid(mesh.data.vertexBuffer.getHandle())) {
      continue;
    }

    size_t vbSize = 0;
    size_t ibSize = 0;
    for (auto &g : mesh.data.geometries) {
      vbSize += g.vertices.size() * sizeof(SkinnedVertex);
      ibSize += g.indices.size() * sizeof(uint32_t);
    }

    {
      rhi::BufferDescription description;
      description.usage = rhi::BufferUsage::Vertex;
      description.size = vbSize;
      description.data = nullptr;
      description.debugName = mesh.name + " vertex";

      mesh.data.vertexBuffer = renderStorage.createBuffer(description);

      auto *data = static_cast<SkinnedVertex *>(mesh.data.vertexBuffer.map());

      size_t offset = 0;
      for (auto &g : mesh.data.geometries) {
        memcpy(data + offset, g.vertices.data(),
               g.vertices.size() * sizeof(SkinnedVertex));
        offset += g.vertices.size();
      }

      mesh.data.vertexBuffer.unmap();
    }

    {
      rhi::BufferDescription description;
      description.usage = rhi::BufferUsage::Index;
      description.size = ibSize;
      description.data = nullptr;
      description.debugName = mesh.name + " index";

      mesh.data.indexBuffer = renderStorage.createBuffer(description);

      auto *data = static_cast<uint32_t *>(mesh.data.indexBuffer.map());
      size_t offset = 0;
      for (auto &g : mesh.data.geometries) {
        memcpy(data + offset, g.indices.data(),
               g.indices.size() * sizeof(uint32_t));
        offset += g.indices.size();
      }

      mesh.data.indexBuffer.unmap();
    }

    mesh.data.materials.resize(mesh.data.geometries.size(), nullptr);
    for (size_t i = 0; i < mesh.data.geometries.size(); ++i) {
      auto &geometry = mesh.data.geometries.at(i);
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

  for (auto &[handle, asset] : mAnimators.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Animator, static_cast<uint32_t>(handle)};
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

  for (auto &[handle, asset] : mEnvironments.getAssets()) {
    if (asset.path == filePath) {
      return {AssetType::Environment, static_cast<uint32_t>(handle)};
    }
  }

  return {AssetType::None, 0};
}

} // namespace liquid
