#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetManager.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<std::filesystem::path>
AssetManager::createMaterialFromAsset(const AssetData<MaterialAsset> &asset) {
  String extension = ".lqmat";

  std::filesystem::path assetPath =
      (mAssetsPath / (asset.name + extension)).make_preferred();

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<std::filesystem::path>::Error(
        "File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.version = createVersion(0, 1);

  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto baseColorTexturePath = getAssetRelativePath(mRegistry.getTextures(),
                                                   asset.data.baseColorTexture);
  file.write(baseColorTexturePath);
  file.write(asset.data.baseColorTextureCoord);
  file.write(asset.data.baseColorFactor);

  auto metallicRoughnessTexturePath = getAssetRelativePath(
      mRegistry.getTextures(), asset.data.metallicRoughnessTexture);
  file.write(metallicRoughnessTexturePath);
  file.write(asset.data.metallicRoughnessTextureCoord);
  file.write(asset.data.metallicFactor);
  file.write(asset.data.roughnessFactor);

  auto normalTexturePath =
      getAssetRelativePath(mRegistry.getTextures(), asset.data.normalTexture);
  file.write(normalTexturePath);
  file.write(asset.data.normalTextureCoord);
  file.write(asset.data.normalScale);

  auto occlusionTexturePath = getAssetRelativePath(mRegistry.getTextures(),
                                                   asset.data.occlusionTexture);
  file.write(occlusionTexturePath);
  file.write(asset.data.occlusionTextureCoord);
  file.write(asset.data.occlusionStrength);

  auto emissiveTexturePath =
      getAssetRelativePath(mRegistry.getTextures(), asset.data.emissiveTexture);
  file.write(emissiveTexturePath);
  file.write(asset.data.emissiveTextureCoord);
  file.write(asset.data.emissiveFactor);

  return Result<std::filesystem::path>::Ok(assetPath);
}

Result<MaterialAssetHandle> AssetManager::loadMaterialDataFromInputStream(
    InputBinaryStream &stream, const std::filesystem::path &filePath) {
  auto assetName = std::filesystem::relative(filePath, mAssetsPath).string();

  AssetData<MaterialAsset> material{};
  material.path = filePath;
  material.name = assetName;
  material.type = AssetType::Material;
  std::vector<String> warnings{};

  // Base color
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res = getOrLoadTextureFromPath(texturePathStr);
    if (res.hasData()) {
      material.data.baseColorTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
    stream.read(material.data.baseColorTextureCoord);
    stream.read(material.data.baseColorFactor);
  }

  // Metallic roughness
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res = getOrLoadTextureFromPath(texturePathStr);
    if (res.hasData()) {
      material.data.metallicRoughnessTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }

    stream.read(material.data.metallicRoughnessTextureCoord);
    stream.read(material.data.metallicFactor);
    stream.read(material.data.roughnessFactor);
  }

  // Normal
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res = getOrLoadTextureFromPath(texturePathStr);
    if (res.hasData()) {
      material.data.normalTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
    stream.read(material.data.normalTextureCoord);
    stream.read(material.data.normalScale);
  }

  // Occlusion
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res = getOrLoadTextureFromPath(texturePathStr);
    if (res.hasData()) {
      material.data.occlusionTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
    stream.read(material.data.occlusionTextureCoord);
    stream.read(material.data.occlusionStrength);
  }

  // Emissive
  {
    String texturePathStr;
    stream.read(texturePathStr);
    const auto &res = getOrLoadTextureFromPath(texturePathStr);
    if (res.hasData()) {
      material.data.emissiveTexture = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
    stream.read(material.data.emissiveTextureCoord);
    stream.read(material.data.emissiveFactor);
  }

  if (material.data.baseColorTexture == TextureAssetHandle::Invalid) {
    warnings.push_back("Base color texture does not exist");
  }

  if (material.data.baseColorTexture == TextureAssetHandle::Invalid) {
    warnings.push_back("Metallic roughness map does not exist");
  }

  if (material.data.baseColorTexture == TextureAssetHandle::Invalid) {
    warnings.push_back("Normal map does not exist");
  }

  if (material.data.baseColorTexture == TextureAssetHandle::Invalid) {
    warnings.push_back("Occlusion map does not exist");
  }

  if (material.data.emissiveTexture == TextureAssetHandle::Invalid) {
    warnings.push_back("Emissive map does not exist");
  }

  return Result<MaterialAssetHandle>::Ok(
      mRegistry.getMaterials().addAsset(material), warnings);
}

Result<MaterialAssetHandle>
AssetManager::loadMaterialFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream stream(filePath);

  if (!stream.good()) {
    return Result<MaterialAssetHandle>::Error(
        "File cannot be opened for reading: " + filePath.string());
  }

  const auto &header = checkAssetFile(stream, filePath, AssetType::Material);
  if (header.hasError()) {
    return Result<MaterialAssetHandle>::Error(header.getError());
  }

  return loadMaterialDataFromInputStream(stream, filePath);
}

Result<MaterialAssetHandle>
AssetManager::getOrLoadMaterialFromPath(const String &relativePath) {
  if (relativePath.empty()) {
    return Result<MaterialAssetHandle>::Ok(MaterialAssetHandle::Invalid);
  }

  std::filesystem::path fullPath =
      (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getMaterials().getAssets()) {
    if (asset.path == fullPath) {
      return Result<MaterialAssetHandle>::Ok(handle);
    }
  }

  return loadMaterialFromFile(fullPath);
}

} // namespace liquid
