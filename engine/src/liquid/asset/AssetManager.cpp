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
    return Result<bool>::Error("File cannot be opened for reading: " +
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

Result<bool> AssetManager::preloadAssets() {
  LIQUID_PROFILE_EVENT("AssetManager::preloadAssets");
  std::vector<String> warnings;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    if (mRegistry.getAssetByPath(entry.path()).first != AssetType::None) {
      continue;
    }

    const auto &ext = entry.path().extension().string();

    if (ext == ".ktx2") {
      loadTextureFromFile(entry.path());
      continue;
    }

    if (ext == ".lua") {
      auto res = loadLuaScriptFromFile(entry.path());
      if (res.hasError()) {
        warnings.push_back(res.getError());
      }
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
      auto res = loadMaterialDataFromInputStream(stream, entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else if (header.type == AssetType::Mesh) {
      auto res = loadMeshDataFromInputStream(stream, entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else if (header.type == AssetType::SkinnedMesh) {
      auto res = loadSkinnedMeshDataFromInputStream(stream, entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else if (header.type == AssetType::Skeleton) {
      auto res = loadSkeletonDataFromInputStream(stream, entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else if (header.type == AssetType::Animation) {
      auto res = loadAnimationDataFromInputStream(stream, entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else if (header.type == AssetType::Prefab) {
      auto res = loadPrefabDataFromInputStream(stream, entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
  }

  return Result<bool>::Ok(true, warnings);
}

String AssetManager::getAssetNameFromPath(const std::filesystem::path &path) {
  auto relativePath = std::filesystem::relative(path, mAssetsPath).string();
  std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
  return relativePath;
}

} // namespace liquid
