#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<void> AssetCache::createMaterialFromData(const MaterialAsset &data,
                                                const Path &assetPath) {
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  auto baseColorTexture = getAssetUuid(data.baseColorTexture);
  file.write(baseColorTexture);
  file.write(data.baseColorTextureCoord);
  file.write(data.baseColorFactor);

  auto metallicRoughnessTexture = getAssetUuid(data.metallicRoughnessTexture);
  file.write(metallicRoughnessTexture);
  file.write(data.metallicRoughnessTextureCoord);
  file.write(data.metallicFactor);
  file.write(data.roughnessFactor);

  auto normalTexture = getAssetUuid(data.normalTexture);
  file.write(normalTexture);
  file.write(data.normalTextureCoord);
  file.write(data.normalScale);

  auto occlusionTexture = getAssetUuid(data.occlusionTexture);
  file.write(occlusionTexture);
  file.write(data.occlusionTextureCoord);
  file.write(data.occlusionStrength);

  auto emissiveTexture = getAssetUuid(data.emissiveTexture);
  file.write(emissiveTexture);
  file.write(data.emissiveTextureCoord);
  file.write(data.emissiveFactor);

  return Ok();
}

Result<MaterialAsset> AssetCache::loadMaterial(const Path &path) {
  InputBinaryStream stream(path);

  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Material) {
    return Error("Invalid file format");
  }

  MaterialAsset material{};
  std::vector<String> warnings{};

  // Base color
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    if (textureUuid.isValid()) {
      auto res = request<TextureAsset>(textureUuid);
      if (res) {
        material.baseColorTexture = res.data().handle();
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
    stream.read(material.baseColorTextureCoord);
    stream.read(material.baseColorFactor);
  }

  // Metallic roughness
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    if (textureUuid.isValid()) {
      auto res = request<TextureAsset>(textureUuid);
      if (res) {
        material.metallicRoughnessTexture = res.data().handle();
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
    stream.read(material.metallicRoughnessTextureCoord);
    stream.read(material.metallicFactor);
    stream.read(material.roughnessFactor);
  }

  // Normal
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    if (textureUuid.isValid()) {
      auto res = request<TextureAsset>(textureUuid);
      if (res) {
        material.normalTexture = res.data().handle();
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
    stream.read(material.normalTextureCoord);
    stream.read(material.normalScale);
  }

  // Occlusion
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    if (textureUuid.isValid()) {
      auto res = request<TextureAsset>(textureUuid);
      if (res) {
        material.occlusionTexture = res.data().handle();
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
    stream.read(material.occlusionTextureCoord);
    stream.read(material.occlusionStrength);
  }

  // Emissive
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    if (textureUuid.isValid()) {
      auto res = request<TextureAsset>(textureUuid);
      if (res) {
        material.emissiveTexture = res.data().handle();
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
    stream.read(material.emissiveTextureCoord);
    stream.read(material.emissiveFactor);
  }

  return {material, warnings};
}

} // namespace quoll
