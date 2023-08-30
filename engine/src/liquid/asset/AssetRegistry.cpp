#include "liquid/core/Base.h"
#include "liquid/renderer/MaterialPBR.h"
#include "liquid/renderer/TextureUtils.h"

#include "AssetRegistry.h"
#include "DefaultObjects.h"

namespace quoll {

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
    if (texture.data.deviceHandle == rhi::TextureHandle::Null) {
      rhi::TextureDescription description{};
      description.width = texture.data.width;
      description.mipLevelCount =
          static_cast<uint32_t>(texture.data.levels.size());
      description.layerCount = texture.data.layers;
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
    if (font.data.deviceHandle == rhi::TextureHandle::Null) {
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
    if (handle != TextureAssetHandle::Null) {
      return mTextures.getAsset(handle).data.deviceHandle;
    }

    return rhi::TextureHandle::Null;
  };

  for (auto &[_, asset] : mMaterials.getAssets()) {
    auto &material = asset.data;
    if (!material.deviceHandle) {
      quoll::MaterialPBR::Properties properties{};

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

      material.deviceHandle.reset(
          new MaterialPBR(asset.name, properties, renderStorage));
    }
  }

  // Synchronize meshes
  for (auto &[_, mesh] : mMeshes.getAssets()) {
    if (!mesh.data.vertexBuffers.empty()) {
      continue;
    }

    size_t ibSize = 0;
    for (auto &g : mesh.data.geometries) {
      ibSize += g.indices.size() * sizeof(uint32_t);
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CreateBuffer(FieldName, Type)                                          \
  {                                                                            \
    size_t vbSize = 0;                                                         \
    for (auto &g : mesh.data.geometries) {                                     \
      size_t vertexSize = g.FieldName.size();                                  \
      vbSize += vertexSize * sizeof(Type);                                     \
    }                                                                          \
    rhi::BufferDescription description;                                        \
    description.usage = rhi::BufferUsage::Vertex;                              \
    description.size = vbSize;                                                 \
    description.data = nullptr;                                                \
    description.debugName = mesh.name + " vertices";                           \
    auto buffer = renderStorage.createBuffer(description);                     \
                                                                               \
    auto *data = static_cast<Type *>(buffer.map());                            \
    mesh.data.vertexBufferOffsets.push_back(0);                                \
    size_t offset = 0;                                                         \
    for (auto &g : mesh.data.geometries) {                                     \
      memcpy(data + offset, g.FieldName.data(),                                \
             g.FieldName.size() * sizeof(Type));                               \
      offset += g.FieldName.size();                                            \
    }                                                                          \
    buffer.unmap();                                                            \
    mesh.data.vertexBuffers.push_back(buffer.getHandle());                     \
  }

    CreateBuffer(positions, glm::vec3);
    CreateBuffer(normals, glm::vec3);
    CreateBuffer(tangents, glm::vec4);
    CreateBuffer(texCoords0, glm::vec2);
    CreateBuffer(texCoords1, glm::vec2);

    if (mesh.type == AssetType::SkinnedMesh) {
      CreateBuffer(joints, glm::uvec4);
      CreateBuffer(weights, glm::vec4);
    }

    {
      rhi::BufferDescription description;
      description.usage = rhi::BufferUsage::Index;
      description.size = ibSize;
      description.data = nullptr;
      description.debugName = mesh.name + " indices";

      auto buffer = renderStorage.createBuffer(description);

      auto *data = static_cast<uint32_t *>(buffer.map());
      size_t offset = 0;
      for (auto &g : mesh.data.geometries) {
        memcpy(data + offset, g.indices.data(),
               g.indices.size() * sizeof(uint32_t));
        offset += g.indices.size();
      }

      buffer.unmap();

      mesh.data.indexBuffer = buffer.getHandle();
    }
  }
}

std::pair<AssetType, uint32_t> AssetRegistry::getAssetByUuid(const Uuid &uuid) {
  LIQUID_PROFILE_EVENT("AssetRegistry::getAssetByUUID");
  for (auto &[handle, asset] : mTextures.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Texture, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mFonts.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Font, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mMaterials.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Material, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mMeshes.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Mesh, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mSkeletons.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Skeleton, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mAnimations.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Animation, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mAnimators.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Animator, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mAudios.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Audio, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mPrefabs.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Prefab, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mLuaScripts.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::LuaScript, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mEnvironments.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Environment, static_cast<uint32_t>(handle)};
    }
  }

  for (auto &[handle, asset] : mScenes.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Scene, static_cast<uint32_t>(handle)};
    }
  }

  return {AssetType::None, 0};
}

} // namespace quoll
