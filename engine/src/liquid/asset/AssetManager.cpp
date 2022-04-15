#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "AssetManager.h"
#include "AssetFileHeader.h"

#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

AssetManager::AssetManager(const std::filesystem::path &assetsPath)
    : mAssetsPath(assetsPath) {}

Result<bool> AssetManager::checkAssetFile(InputBinaryStream &file,
                                          const std::filesystem::path &filePath,
                                          AssetType assetType) {
  if (!file.good()) {
    return Result<bool>::Error("File cannot be opened for writing: " +
                               filePath.string());
  }

  AssetFileHeader header;
  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  if (magic != header.magic) {
    return Result<bool>::Error("Opened file is not a liquid asset: " +
                               filePath.string());
  }

  if (header.type != assetType) {
    return Result<bool>::Error("Opened file is not a liquid " +
                               getAssetTypeString(assetType) +
                               " asset: " + filePath.string());
  }

  return Result<bool>::Ok(true);
}

} // namespace liquid
