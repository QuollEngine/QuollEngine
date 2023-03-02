#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "liquid/asset/AssetCache.h"
#include "liquid/asset/InputBinaryStream.h"
#include "liquid/asset/OutputBinaryStream.h"
#include "liquid/asset/AssetFileHeader.h"

namespace liquid {

Result<Path> AssetCache::createEnvironmentFromAsset(
    const AssetData<EnvironmentAsset> &asset) {
  auto irradianceMapPath = mRegistry.getTextures()
                               .getAsset(asset.data.irradianceMap)
                               .relativePath.string();
  std::replace(irradianceMapPath.begin(), irradianceMapPath.end(), '\\', '/');

  auto specularMapPath = mRegistry.getTextures()
                             .getAsset(asset.data.specularMap)
                             .relativePath.string();
  std::replace(specularMapPath.begin(), specularMapPath.end(), '\\', '/');

  OutputBinaryStream stream(asset.path);
  AssetFileHeader header{};
  header.type = AssetType::Environment;
  header.version = createVersion(0, 1);
  stream.write(header.magic, AssetFileMagicLength);
  stream.write(header.version);
  stream.write(header.type);

  stream.write(irradianceMapPath);
  stream.write(specularMapPath);

  return Result<Path>::Ok(asset.path);
}

Result<EnvironmentAssetHandle>
AssetCache::loadEnvironmentDataFromInputStream(InputBinaryStream &stream,
                                               const Path &filePath) {
  String irradianceMapPath;
  stream.read(irradianceMapPath);

  String specularMapPath;
  stream.read(specularMapPath);

  auto irradianceMapRes = getOrLoadTextureFromPath(irradianceMapPath);
  if (irradianceMapRes.hasError()) {
    return Result<EnvironmentAssetHandle>::Error(irradianceMapRes.getError());
  }

  auto specularMapRes = getOrLoadTextureFromPath(specularMapPath);
  if (specularMapRes.hasError()) {
    mRegistry.getTextures().deleteAsset(irradianceMapRes.getData());
    return Result<EnvironmentAssetHandle>::Error(specularMapRes.getError());
  }

  AssetData<EnvironmentAsset> environment{};
  environment.path = filePath;
  environment.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  environment.name = environment.relativePath.string();
  environment.type = AssetType::Environment;
  environment.data.irradianceMap = irradianceMapRes.getData();
  environment.data.specularMap = specularMapRes.getData();

  auto environmentHandle = mRegistry.getEnvironments().addAsset(environment);

  return Result<EnvironmentAssetHandle>::Ok(environmentHandle);
}

Result<EnvironmentAssetHandle>
AssetCache::loadEnvironmentFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Environment);
  if (header.hasError()) {
    return Result<EnvironmentAssetHandle>::Error(header.getError());
  }

  return loadEnvironmentDataFromInputStream(stream, filePath);
}

} // namespace liquid
