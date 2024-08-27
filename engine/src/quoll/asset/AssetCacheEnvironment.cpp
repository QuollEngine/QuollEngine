#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll/asset/OutputBinaryStream.h"

namespace quoll {

Result<Path> AssetCache::createEnvironmentFromAsset(
    const AssetData<EnvironmentAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(AssetType::Environment, asset.name, assetPath);
  if (!metaRes) {
    return Error("Cannot create environment asset: " + asset.name);
  }

  auto irradianceMapUuid = mRegistry.getMeta(asset.data.irradianceMap).uuid;
  auto specularMapUuid = mRegistry.getMeta(asset.data.specularMap).uuid;

  OutputBinaryStream stream(assetPath);
  AssetFileHeader header{};
  header.type = AssetType::Environment;
  header.magic = AssetFileHeader::MagicConstant;
  stream.write(header);

  stream.write(irradianceMapUuid);
  stream.write(specularMapUuid);

  return assetPath;
}

Result<EnvironmentAsset>
AssetCache::loadEnvironmentDataFromInputStream(const Path &path) {
  InputBinaryStream stream(path);

  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Environment) {
    return Error("Invalid file format");
  }

  Uuid irradianceMapUuid;
  stream.read(irradianceMapUuid);

  Uuid specularMapUuid;
  stream.read(specularMapUuid);

  auto irradianceMapRes = getOrLoad<TextureAsset>(irradianceMapUuid);
  if (!irradianceMapRes) {
    return irradianceMapRes.error();
  }

  auto specularMapRes = getOrLoad<TextureAsset>(specularMapUuid);
  if (!specularMapRes) {
    mRegistry.remove(irradianceMapRes.data());
    return specularMapRes.error();
  }

  EnvironmentAsset environment{};
  environment.irradianceMap = irradianceMapRes;
  environment.specularMap = specularMapRes;
  return environment;
}

Result<EnvironmentAsset> AssetCache::loadEnvironment(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Environment) {
    return Error("Asset type is not environment");
  }

  return loadEnvironmentDataFromInputStream(getPathFromUuid(uuid));
}

} // namespace quoll
