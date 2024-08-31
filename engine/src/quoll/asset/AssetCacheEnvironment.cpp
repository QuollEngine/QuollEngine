#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll/asset/OutputBinaryStream.h"

namespace quoll {

Result<void> AssetCache::createEnvironmentFromData(const EnvironmentAsset &data,
                                                   const Path &assetPath) {
  OutputBinaryStream stream(assetPath);
  AssetFileHeader header{};
  header.type = AssetType::Environment;
  header.magic = AssetFileHeader::MagicConstant;
  stream.write(header);

  stream.write(data.irradianceMap.meta().uuid);
  stream.write(data.specularMap.meta().uuid);

  return Ok();
}

Result<EnvironmentAsset> AssetCache::loadEnvironment(const Path &path) {
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
  environment.irradianceMap = irradianceMapRes.data();
  environment.specularMap = specularMapRes.data();
  return environment;
}

} // namespace quoll
