#include "quoll/core/Base.h"
#include "quoll/rhi/TextureDescription.h"
#include "MaterialPBR.h"
#include "RenderStorage.h"
#include "RendererAssetRegistry.h"
#include "TextureUtils.h"

namespace quoll {

RendererAssetRegistry::RendererAssetRegistry(RenderStorage &storage)
    : mStorage(storage) {}

rhi::TextureHandle
RendererAssetRegistry::get(const AssetRef<TextureAsset> &asset) {
  auto it = mTextures.find(asset.handle());
  if (it != mTextures.end()) {
    return it->second;
  }

  const auto &texture = asset.get();

  rhi::TextureDescription description{};
  description.width = texture.width;
  description.mipLevelCount = static_cast<u32>(texture.levels.size());
  description.layerCount = texture.layers;
  description.height = texture.height;
  description.usage = rhi::TextureUsage::Color |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Sampled;
  description.type = texture.type == TextureAssetType::Cubemap
                         ? rhi::TextureType::Cubemap
                         : rhi::TextureType::Standard;
  description.format = texture.format;
  description.debugName = asset.meta().name;

  auto handle = mStorage.createTexture(description);
  TextureUtils::copyDataToTexture(
      mStorage.getDevice(), texture.data.data(), handle,
      rhi::ImageLayout::ShaderReadOnlyOptimal, texture.layers, texture.levels);

  mTextures.insert_or_assign(asset.handle(), handle);

  return handle;
}

Material *RendererAssetRegistry::get(const AssetRef<MaterialAsset> &asset) {
  auto it = mMaterials.find(asset.handle());
  if (it != mMaterials.end()) {
    return it->second.get();
  }

  auto getTextureFromRegistry = [this](const AssetRef<TextureAsset> &texture) {
    return texture ? get(texture) : rhi::TextureHandle::Null;
  };

  const auto &material = asset.get();
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
  properties.emissiveTexture = getTextureFromRegistry(material.emissiveTexture);
  properties.emissiveTextureCoord = material.emissiveTextureCoord;

  auto *deviceObject = new MaterialPBR(asset.meta().name, properties, mStorage);

  mMaterials.insert_or_assign(asset.handle(),
                              std::unique_ptr<Material>(deviceObject));

  return deviceObject;
}

const MeshDrawData &
RendererAssetRegistry::get(const AssetRef<MeshAsset> &asset) {
  auto it = mMeshBuffers.find(asset.handle());
  if (it != mMeshBuffers.end()) {
    return it->second;
  }

  const auto &mesh = asset.get();

  usize ibSize = 0;
  for (auto &g : mesh.geometries) {
    ibSize += g.indices.size() * sizeof(u32);
  }

  MeshDrawData drawData{};

  for (auto &g : mesh.geometries) {
    drawData.geometries.push_back(
        {.numVertices = static_cast<u32>(g.positions.size()),
         .numIndices = static_cast<u32>(g.indices.size())});
  }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CreateBuffer(FieldName, Type)                                          \
  {                                                                            \
    usize vbSize = 0;                                                          \
    for (auto &g : mesh.geometries) {                                          \
      usize vertexSize = g.FieldName.size();                                   \
      vbSize += vertexSize * sizeof(Type);                                     \
    }                                                                          \
                                                                               \
    if (vbSize > 0) {                                                          \
      rhi::BufferDescription description;                                      \
      description.usage = rhi::BufferUsage::Vertex;                            \
      description.size = vbSize;                                               \
      description.data = nullptr;                                              \
      description.debugName = asset.meta().name + " " #FieldName;              \
      auto buffer = mStorage.createBuffer(description);                        \
      auto *data = static_cast<Type *>(buffer.map());                          \
      drawData.vertexBufferOffsets.push_back(0);                               \
      usize offset = 0;                                                        \
      for (auto &g : mesh.geometries) {                                        \
        memcpy(data + offset, g.FieldName.data(),                              \
               g.FieldName.size() * sizeof(Type));                             \
        offset += g.FieldName.size();                                          \
      }                                                                        \
      buffer.unmap();                                                          \
      drawData.vertexBuffers.push_back(buffer.getHandle());                    \
    }                                                                          \
  }

  CreateBuffer(positions, glm::vec3);
  CreateBuffer(normals, glm::vec3);
  CreateBuffer(tangents, glm::vec4);
  CreateBuffer(texCoords0, glm::vec2);
  CreateBuffer(texCoords1, glm::vec2);
  CreateBuffer(joints, glm::uvec4);
  CreateBuffer(weights, glm::vec4);

  {
    rhi::BufferDescription description;
    description.usage = rhi::BufferUsage::Index;
    description.size = ibSize;
    description.data = nullptr;
    description.debugName = asset.meta().name + " indices";

    auto buffer = mStorage.createBuffer(description);

    auto *data = static_cast<u32 *>(buffer.map());
    usize offset = 0;
    for (auto &g : mesh.geometries) {
      memcpy(data + offset, g.indices.data(), g.indices.size() * sizeof(u32));
      offset += g.indices.size();
    }

    buffer.unmap();

    drawData.indexBuffer = buffer.getHandle();
  }

  mMeshBuffers.insert_or_assign(asset.handle(), std::move(drawData));

  return mMeshBuffers.at(asset.handle());
}

rhi::TextureHandle
RendererAssetRegistry::get(const AssetRef<FontAsset> &asset) {
  auto it = mFontAtlases.find(asset.handle());
  if (it != mFontAtlases.end()) {
    return it->second;
  }

  const auto &font = asset.get();

  rhi::TextureDescription description{};
  description.width = font.atlasDimensions.x;
  description.height = font.atlasDimensions.y;
  description.usage = rhi::TextureUsage::Color |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Sampled;
  description.format = rhi::Format::Rgba8Unorm;
  description.debugName = asset.meta().name;

  auto handle = mStorage.createTexture(description);
  mFontAtlases.insert_or_assign(asset.handle(), handle);

  TextureUtils::copyDataToTexture(
      mStorage.getDevice(), font.atlasBytes.data(), handle,
      rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {{0, font.size, font.atlasDimensions.x, font.atlasDimensions.y}});

  return handle;
}

} // namespace quoll
