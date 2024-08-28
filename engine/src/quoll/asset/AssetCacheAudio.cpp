#include "quoll/core/Base.h"
#include "quoll/audio/MiniAudio.h"
#include "AssetCache.h"

namespace quoll {

static AudioAssetFormat getAudioFormatFromExtension(StringView extension) {
  if (extension == "wav") {
    return AudioAssetFormat::Wav;
  }

  return AudioAssetFormat::Unknown;
}

Result<AudioAsset> AssetCache::loadAudio(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  auto ext = filePath.extension().string();
  ext.erase(0, 1);

  auto *decoder = new ma_decoder;

  std::ifstream stream(filePath, std::ios::binary | std::ios::ate);

  if (stream.bad()) {
    return Error("Cannot load audio file: " + filePath.string());
  }

  std::ifstream::pos_type pos = stream.tellg();

  if (pos <= 0) {
    return Error("Could not open file: File is empty");
  }

  std::vector<char> bytes(pos);
  stream.seekg(0, std::ios::beg);
  stream.read(&bytes[0], pos);

  auto meta = getAssetMeta(uuid);

  AudioAsset asset;
  asset.bytes = bytes;

  return asset;
}

} // namespace quoll
