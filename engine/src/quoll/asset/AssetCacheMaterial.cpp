#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createMaterialFromAsset(const AssetData<MaterialAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(AssetType::Material, asset.name, assetPath);
  if (!metaRes.hasData()) {
    return Result<Path>::Error("Cannot create material asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
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

  return Result<Path>::Ok(assetPath);
}

Result<AssetHandle<MaterialAsset>>
AssetCache::loadMaterialDataFromInputStream(InputBinaryStream &stream,
                                            const Path &filePath,
                                            const AssetFileHeader &header) {

  AssetData<MaterialAsset> material{};
  material.name = header.name;
  material.path = filePath;
  material.uuid = Uuid(filePath.stem().string());
  material.type = AssetType::Material;
  std::vector<String> warnings{};

  // Base color
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    const auto &res = getOrLoadTexture(textureUuid);
    if (res.hasData()) {
      material.data.baseColorTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.baseColorTextureCoord);
    stream.read(material.data.baseColorFactor);
  }

  // Metallic roughness
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    const auto &res = getOrLoadTexture(textureUuid);
    if (res.hasData()) {
      material.data.metallicRoughnessTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }

    stream.read(material.data.metallicRoughnessTextureCoord);
    stream.read(material.data.metallicFactor);
    stream.read(material.data.roughnessFactor);
  }

  // Normal
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    const auto &res = getOrLoadTexture(textureUuid);
    if (res.hasData()) {
      material.data.normalTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.normalTextureCoord);
    stream.read(material.data.normalScale);
  }

  // Occlusion
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    const auto &res = getOrLoadTexture(textureUuid);
    if (res.hasData()) {
      material.data.occlusionTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.occlusionTextureCoord);
    stream.read(material.data.occlusionStrength);
  }

  // Emissive
  {
    Uuid textureUuid;
    stream.read(textureUuid);
    const auto &res = getOrLoadTexture(textureUuid);
    if (res.hasData()) {
      material.data.emissiveTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
    stream.read(material.data.emissiveTextureCoord);
    stream.read(material.data.emissiveFactor);
  }

  return Result<AssetHandle<MaterialAsset>>::Ok(mRegistry.add(material),
                                                warnings);
}

Result<AssetHandle<MaterialAsset>> AssetCache::loadMaterial(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);
  InputBinaryStream stream(filePath);

  if (!stream.good()) {
    return Result<AssetHandle<MaterialAsset>>::Error(
        "File cannot be opened for reading: " + filePath.string());
  }

  const auto &header = checkAssetFile(stream, filePath, AssetType::Material);
  if (header.hasError()) {
    return Result<AssetHandle<MaterialAsset>>::Error(header.getError());
  }

  return loadMaterialDataFromInputStream(stream, filePath, header.getData());
}

Result<AssetHandle<MaterialAsset>>
AssetCache::getOrLoadMaterial(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<AssetHandle<MaterialAsset>>::Ok(AssetHandle<MaterialAsset>());
  }

  auto handle = mRegistry.findHandleByUuid<MaterialAsset>(uuid);
  if (handle) {
    return Result<AssetHandle<MaterialAsset>>::Ok(handle);
  }

  return loadMaterial(uuid);
}

} // namespace quoll
