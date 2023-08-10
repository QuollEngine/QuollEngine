#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"

#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

AssetCache::AssetCache(const Path &assetsPath, bool createDefaultObjects)
    : mAssetsPath(assetsPath) {
  if (createDefaultObjects) {
    mRegistry.createDefaultObjects();
  }
}

Result<bool> AssetCache::checkAssetFile(InputBinaryStream &file,
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

Result<bool> AssetCache::preloadAssets(RenderStorage &renderStorage) {
  LIQUID_PROFILE_EVENT("AssetCache::preloadAssets");
  std::vector<String> warnings;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file() || entry.path().extension() == ".lqhash" ||
        entry.path().extension() == ".assetmeta") {
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

  mRegistry.syncWithDevice(renderStorage);

  return Result<bool>::Ok(true, warnings);
}

Result<bool> AssetCache::loadAsset(const Path &path) {
  return loadAsset(path, true);
}

AssetType AssetCache::getTypeFromAssetPath(const Path &path) const {
  AssetType type = AssetType::None;
  auto typePath = path;
  typePath.replace_extension("assetmeta");
  if (!std::filesystem::exists(typePath)) {
    return type;
  }

  std::ifstream stream(typePath, std::ios::binary);
  if (stream.good()) {
    stream.read(reinterpret_cast<char *>(&type), sizeof(AssetType));
    stream.close();
  }
  return type;
}

Result<bool> AssetCache::loadAsset(const Path &path, bool updateExisting) {
  const auto &ext = path.extension().string();
  const auto &asset = mRegistry.getAssetByPath(path);

  uint32_t handle = updateExisting ? asset.second : 0;

  if (updateExisting && asset.first != AssetType::None &&
      asset.first != AssetType::LuaScript &&
      asset.first != AssetType::Animator) {
    return Result<bool>::Error(
        "Can only reload Lua scripts and animators on watch");
  }

  // Handle files that are not in liquid format
  auto nonLiquidAssetType = getTypeFromAssetPath(path);

  if (nonLiquidAssetType == AssetType::Texture) {
    auto res = loadTextureFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (nonLiquidAssetType == AssetType::LuaScript) {
    auto res =
        loadLuaScriptFromFile(path, static_cast<LuaScriptAssetHandle>(handle));
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (nonLiquidAssetType == AssetType::Animator) {
    auto res =
        loadAnimatorFromFile(path, static_cast<AnimatorAssetHandle>(handle));
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (nonLiquidAssetType == AssetType::Audio) {
    auto res = loadAudioFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (nonLiquidAssetType == AssetType::Font) {
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
    return Result<bool>::Error("Not a liquid asset: " + path.stem().string());
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

  if (header.type == AssetType::Environment) {
    auto res = loadEnvironmentDataFromInputStream(stream, path);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  return Result<bool>::Error("Unknown asset file: " + path.stem().string());
}

String AssetCache::getAssetNameFromPath(const Path &path) {
  auto relativePath = std::filesystem::relative(path, mAssetsPath).string();
  std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
  return relativePath;
}

Result<Path> AssetCache::createMetaFile(AssetType type, Path path) {
  auto metaPath = path.replace_extension("assetmeta");
  std::ofstream out(metaPath, std::ios::binary);

  if (!out.good()) {
    return Result<Path>::Error("Cannot create meta file for asset: " +
                               path.stem().string());
  }

  out.write(reinterpret_cast<const char *>(&type), sizeof(type));
  out.close();

  return Result<Path>::Ok(metaPath);
}

} // namespace liquid
