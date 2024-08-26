#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createMaterialFromAsset(const AssetData<MaterialAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(AssetType::Material, asset.name, assetPath);
  if (!metaRes) {
    return Error("Cannot create material asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  auto baseColorTexture = getAssetUuid(asset.data.baseColorTexture);
  file.write(baseColorTexture);
  file.write(asset.data.baseColorTextureCoord);
  file.write(asset.data.baseColorFactor);

  auto metallicRoughnessTexture =
      getAssetUuid(asset.data.metallicRoughnessTexture);
  file.write(metallicRoughnessTexture);
  file.write(asset.data.metallicRoughnessTextureCoord);
  file.write(asset.data.metallicFactor);
  file.write(asset.data.roughnessFactor);

  auto normalTexture = getAssetUuid(asset.data.normalTexture);
  file.write(normalTexture);
  file.write(asset.data.normalTextureCoord);
  file.write(asset.data.normalScale);

  auto occlusionTexture = getAssetUuid(asset.data.occlusionTexture);
  file.write(occlusionTexture);
  file.write(asset.data.occlusionTextureCoord);
  file.write(asset.data.occlusionStrength);

  auto emissiveTexture = getAssetUuid(asset.data.emissiveTexture);
  file.write(emissiveTexture);
  file.write(asset.data.emissiveTextureCoord);
  file.write(asset.data.emissiveFactor);

  return assetPath;
}

Result<MaterialAsset>
AssetCache::loadMaterialDataFromInputStream(const Path &path) {
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
    auto res = getOrLoad<TextureAsset>(textureUuid);
    if (res) {
      material.baseColorTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.baseColorTextureCoord);
    stream.read(material.baseColorFactor);
  }

  // Metallic roughness
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoad<TextureAsset>(textureUuid);
    if (res) {
      material.metallicRoughnessTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }

    stream.read(material.metallicRoughnessTextureCoord);
    stream.read(material.metallicFactor);
    stream.read(material.roughnessFactor);
  }

  // Normal
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoad<TextureAsset>(textureUuid);
    if (res) {
      material.normalTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.normalTextureCoord);
    stream.read(material.normalScale);
  }

  // Occlusion
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoad<TextureAsset>(textureUuid);
    if (res) {
      material.occlusionTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.occlusionTextureCoord);
    stream.read(material.occlusionStrength);
  }

  // Emissive
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoad<TextureAsset>(textureUuid);
    if (res) {
      material.emissiveTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.emissiveTextureCoord);
    stream.read(material.emissiveFactor);
  }

  return {material, warnings};
}

Result<MaterialAsset> AssetCache::loadMaterial(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Material) {
    return Error("Asset type is not material");
  }

  return loadMaterialDataFromInputStream(getPathFromUuid(uuid));
}

} // namespace quoll
