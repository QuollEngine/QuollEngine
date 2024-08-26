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

Result<AssetHandle<MaterialAsset>>
AssetCache::loadMaterialDataFromInputStream(const Path &path, const Uuid &uuid,
                                            const AssetMeta &meta) {
  InputBinaryStream stream(path);

  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Material) {
    return Error("Invalid file format");
  }

  AssetData<MaterialAsset> material{};
  material.name = meta.name;
  material.uuid = uuid;
  material.type = AssetType::Material;
  std::vector<String> warnings{};

  // Base color
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoadTexture(textureUuid);
    if (res) {
      material.data.baseColorTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.data.baseColorTextureCoord);
    stream.read(material.data.baseColorFactor);
  }

  // Metallic roughness
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoadTexture(textureUuid);
    if (res) {
      material.data.metallicRoughnessTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }

    stream.read(material.data.metallicRoughnessTextureCoord);
    stream.read(material.data.metallicFactor);
    stream.read(material.data.roughnessFactor);
  }

  // Normal
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoadTexture(textureUuid);
    if (res) {
      material.data.normalTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.data.normalTextureCoord);
    stream.read(material.data.normalScale);
  }

  // Occlusion
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoadTexture(textureUuid);
    if (res) {
      material.data.occlusionTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.data.occlusionTextureCoord);
    stream.read(material.data.occlusionStrength);
  }

  // Emissive
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    auto res = getOrLoadTexture(textureUuid);
    if (res) {
      material.data.emissiveTexture = res;
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    } else {
      warnings.push_back(res.error());
    }
    stream.read(material.data.emissiveTextureCoord);
    stream.read(material.data.emissiveFactor);
  }

  return {mRegistry.add(material), warnings};
}

Result<AssetHandle<MaterialAsset>> AssetCache::loadMaterial(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Material) {
    return Error("Asset type is not material");
  }

  return loadMaterialDataFromInputStream(getPathFromUuid(uuid), uuid, meta);
}

Result<AssetHandle<MaterialAsset>>
AssetCache::getOrLoadMaterial(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return AssetHandle<MaterialAsset>();
  }

  auto handle = mRegistry.findHandleByUuid<MaterialAsset>(uuid);
  if (handle) {
    return handle;
  }

  return loadMaterial(uuid);
}

} // namespace quoll
