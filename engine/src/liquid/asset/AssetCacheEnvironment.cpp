#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "liquid/asset/AssetCache.h"
#include "liquid/asset/InputBinaryStream.h"
#include "liquid/asset/OutputBinaryStream.h"
#include "liquid/asset/AssetFileHeader.h"

namespace liquid {

Result<Path> AssetCache::createEnvironmentFromAsset(
    const AssetData<EnvironmentAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    LIQUID_ASSERT(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto irradianceMapUuid =
      mRegistry.getTextures().getAsset(asset.data.irradianceMap).uuid;

  auto specularMapUuid =
      mRegistry.getTextures().getAsset(asset.data.specularMap).uuid;

  auto assetPath = getPathFromUuid(asset.uuid);

  OutputBinaryStream stream(assetPath);
  AssetFileHeader header{};
  header.type = AssetType::Environment;
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
  stream.write(header);

  stream.write(irradianceMapUuid);
  stream.write(specularMapUuid);

  return Result<Path>::Ok(assetPath);
}

Result<EnvironmentAssetHandle>
AssetCache::loadEnvironmentDataFromInputStream(InputBinaryStream &stream,
                                               const Path &filePath,
                                               const AssetFileHeader &header) {
  Uuid irradianceMapUuid;
  stream.read(irradianceMapUuid);

  Uuid specularMapUuid;
  stream.read(specularMapUuid);

  auto irradianceMapRes = getOrLoadTexture(irradianceMapUuid);
  if (irradianceMapRes.hasError()) {
    return Result<EnvironmentAssetHandle>::Error(irradianceMapRes.getError());
  }

  auto specularMapRes = getOrLoadTexture(specularMapUuid);
  if (specularMapRes.hasError()) {
    mRegistry.getTextures().deleteAsset(irradianceMapRes.getData());
    return Result<EnvironmentAssetHandle>::Error(specularMapRes.getError());
  }

  AssetData<EnvironmentAsset> environment{};
  environment.path = filePath;
  environment.uuid = Uuid(filePath.stem().string());
  environment.type = AssetType::Environment;
  environment.data.irradianceMap = irradianceMapRes.getData();
  environment.data.specularMap = specularMapRes.getData();

  auto environmentHandle = mRegistry.getEnvironments().addAsset(environment);

  return Result<EnvironmentAssetHandle>::Ok(environmentHandle);
}

Result<EnvironmentAssetHandle> AssetCache::loadEnvironment(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Environment);
  if (header.hasError()) {
    return Result<EnvironmentAssetHandle>::Error(header.getError());
  }

  return loadEnvironmentDataFromInputStream(stream, filePath, header.getData());
}

} // namespace liquid
