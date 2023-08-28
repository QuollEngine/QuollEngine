#include "liquid/core/Base.h"
#include "liquid/audio/MiniAudio.h"
#include "AssetCache.h"

namespace liquid {

static AudioAssetFormat getAudioFormatFromExtension(StringView extension) {
  if (extension == "wav") {
    return AudioAssetFormat::Wav;
  }

  return AudioAssetFormat::Unknown;
}

Result<Path> AssetCache::createAudioFromSource(const Path &sourcePath,
                                               const Uuid &uuid) {
  if (uuid.isEmpty()) {
    LIQUID_ASSERT(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  using co = std::filesystem::copy_options;

  auto assetPath = getPathFromUuid(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Result<Path>::Error("Cannot create audio from source: " +
                               sourcePath.stem().string());
  }

  auto metaRes = createAssetMeta(AssetType::Audio,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create audio from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

Result<AudioAssetHandle> AssetCache::loadAudio(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  auto ext = filePath.extension().string();
  ext.erase(0, 1);

  auto *decoder = new ma_decoder;

  std::ifstream stream(filePath, std::ios::binary | std::ios::ate);

  if (stream.bad()) {
    return Result<AudioAssetHandle>::Error("Cannot load audio file: " +
                                           filePath.string());
  }

  std::ifstream::pos_type pos = stream.tellg();

  if (pos <= 0) {
    return Result<AudioAssetHandle>::Error(
        "Could not open file: File is empty");
  }

  std::vector<char> bytes(pos);
  stream.seekg(0, std::ios::beg);
  stream.read(&bytes[0], pos);

  auto meta = getAssetMeta(uuid);

  AssetData<AudioAsset> asset;
  asset.name = meta.name;
  asset.path = filePath;
  asset.uuid = Uuid(filePath.stem().string());
  asset.type = AssetType::Audio;
  asset.data.bytes = bytes;

  return Result<AudioAssetHandle>::Ok(mRegistry.getAudios().addAsset(asset));
}

} // namespace liquid
