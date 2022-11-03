#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "AssetManager.h"
#include "AssetFileHeader.h"

#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

AssetManager::AssetManager(const Path &assetsPath, bool createDefaultObjects)
    : mAssetsPath(assetsPath) {
  if (createDefaultObjects) {
    mRegistry.createDefaultObjects();
  }
}

Result<bool> AssetManager::checkAssetFile(InputBinaryStream &file,
                                          const Path &filePath,
                                          AssetType assetType) {
  if (!file.good()) {
    return Result<bool>::Error("File cannot be opened for reading: " +
                               filePath.string());
  }

  AssetFileHeader header;
  String magic(AssetFileMagicLength, '$');
  file.read(magic.data(), AssetFileMagicLength);
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

Result<bool> AssetManager::preloadAssets(rhi::RenderDevice *device) {
  LIQUID_PROFILE_EVENT("AssetManager::preloadAssets");
  std::vector<String> warnings;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file() || entry.path().extension() == ".lqhash") {
      continue;
    }

    auto res = loadAsset(entry.path(), false);

    if (res.hasError()) {
      warnings.push_back(res.getError());
    } else {
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
  }

  mRegistry.syncWithDevice(device);

  return Result<bool>::Ok(true, warnings);
}

Result<bool> AssetManager::loadAsset(const Path &path) {
  return loadAsset(path, true);
}

Result<bool> AssetManager::loadAsset(const Path &path, bool updateExisting) {
  const auto &ext = path.extension().string();
  const auto &asset = mRegistry.getAssetByPath(path);

  uint32_t handle = updateExisting ? asset.second : 0;

  if (updateExisting && asset.first != AssetType::None &&
      asset.first != AssetType::LuaScript) {
    return Result<bool>::Error("Can only reload Lua scripts on watch");
  }

  if (ext == ".ktx2") {
    auto res = loadTextureFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (ext == ".lua") {
    auto res =
        loadLuaScriptFromFile(path, static_cast<LuaScriptAssetHandle>(handle));
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (ext == ".wav" || ext == ".mp3" || ext == ".flac") {
    auto res = loadAudioFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (ext == ".ttf" || ext == ".otf") {
    auto res = loadFontFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  InputBinaryStream stream(path);
  AssetFileHeader header;
  String magic(AssetFileMagicLength, '$');
  stream.read(magic.data(), AssetFileMagicLength);

  if (magic != header.magic) {
    return Result<bool>::Error("Not a liquid asset");
  }

  stream.read(header.version);
  stream.read(header.type);

  if (header.type == AssetType::Material) {
    auto res = loadMaterialDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Mesh) {
    auto res = loadMeshDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::SkinnedMesh) {
    auto res = loadSkinnedMeshDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Skeleton) {
    auto res = loadSkeletonDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Animation) {
    auto res = loadAnimationDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Prefab) {
    auto res = loadPrefabDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  return Result<bool>::Error("Unknown asset file");
}

String AssetManager::getAssetNameFromPath(const Path &path) {
  auto relativePath = std::filesystem::relative(path, mAssetsPath).string();
  std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
  return relativePath;
}

} // namespace liquid
