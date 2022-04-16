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

void AssetManager::preloadAssets() {
  LIQUID_PROFILE_EVENT("AssetManager::preloadAssets");
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    if (mRegistry.getAssetType(entry.path()) != AssetType::None) {
      continue;
    }

    const auto &ext = entry.path().extension().string();

    if (ext == ".ktx2") {
      loadTextureFromFile(entry.path());
      continue;
    }

    InputBinaryStream stream(entry.path());
    AssetFileHeader header;
    String magic(ASSET_FILE_MAGIC_LENGTH, '$');
    stream.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);

    if (magic != header.magic) {
      continue;
    }

    stream.read(header.version);
    stream.read(header.type);

    if (header.type == AssetType::Material) {
      loadMaterialDataFromInputStream(stream, entry.path());
    } else if (header.type == AssetType::Mesh) {
      loadMeshDataFromInputStream(stream, entry.path());
    } else if (header.type == AssetType::SkinnedMesh) {
      loadSkinnedMeshDataFromInputStream(stream, entry.path());
    } else if (header.type == AssetType::Skeleton) {
      loadSkeletonDataFromInputStream(stream, entry.path());
    } else if (header.type == AssetType::Animation) {
      loadAnimationDataFromInputStream(stream, entry.path());
    } else if (header.type == AssetType::Prefab) {
      loadPrefabDataFromInputStream(stream, entry.path());
    }
  }
}

} // namespace liquid
