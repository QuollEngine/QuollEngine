#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll/asset/OutputBinaryStream.h"

namespace quoll {

Result<void> AssetCache::createEnvironmentFromData(const EnvironmentAsset &data,
                                                   const Path &assetPath) {
  auto irradianceMapUuid = mRegistry.getMeta(data.irradianceMap).uuid;
  auto specularMapUuid = mRegistry.getMeta(data.specularMap).uuid;

  OutputBinaryStream stream(assetPath);
  AssetFileHeader header{};
  header.type = AssetType::Environment;
  header.magic = AssetFileHeader::MagicConstant;
  stream.write(header);

  stream.write(irradianceMapUuid);
  stream.write(specularMapUuid);

  return Ok();
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

  auto irradianceMapRes = request<TextureAsset>(irradianceMapUuid);
  if (!irradianceMapRes) {
    return irradianceMapRes.error();
  }

  auto specularMapRes = request<TextureAsset>(specularMapUuid);
  if (!specularMapRes) {
    mRegistry.remove(irradianceMapRes.data().handle());
    return specularMapRes.error();
  }

  EnvironmentAsset environment{};
  environment.irradianceMap = irradianceMapRes.data().handle();
  environment.specularMap = specularMapRes.data().handle();
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
