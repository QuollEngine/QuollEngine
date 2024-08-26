#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

AssetCache::AssetCache(const Path &assetsPath, bool createDefaultObjects)
    : mAssetsPath(assetsPath) {
  if (createDefaultObjects) {
    mRegistry.createDefaultObjects();
  }
}

Result<AssetFileHeader> AssetCache::checkAssetFile(InputBinaryStream &file,
                                                   const Path &filePath,
                                                   AssetType assetType) {
  if (!file.good()) {
    return Result<AssetFileHeader>::Error(
        "File cannot be opened for reading: " + filePath.string());
  }

  AssetFileHeader header;
  file.read(header);

  if (header.magic != AssetFileHeader::MagicConstant) {
    return Result<AssetFileHeader>::Error("Opened file is not a quoll asset: " +
                                          filePath.string());
  }

  if (assetType != AssetType::None && header.type != assetType) {
    return Result<AssetFileHeader>::Error("Opened file is not a quoll " +
                                          getAssetTypeString(assetType) +
                                          " asset: " + filePath.string());
  }

  return Result<AssetFileHeader>::Ok(header);
}

Result<bool> AssetCache::preloadAssets(RenderStorage &renderStorage) {
  QUOLL_PROFILE_EVENT("AssetCache::preloadAssets");
  std::vector<String> warnings;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file() || entry.path().extension() == ".assetmeta") {
      continue;
    }

    auto res = loadAsset(entry.path());

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

AssetMeta AssetCache::getAssetMeta(const Uuid &uuid) const {
  AssetMeta meta{};
  auto typePath =
      (mAssetsPath / uuid.toString()).replace_extension("assetmeta");
  if (!std::filesystem::exists(typePath)) {
    return meta;
  }

  InputBinaryStream stream(typePath);
  if (!stream.good()) {
    return meta;
  }

  stream.read(meta);
  return meta;
}

Result<bool> AssetCache::loadAsset(const Path &path) {
  auto uuid = Uuid(path.stem().string());

  auto meta = getAssetMeta(uuid);

  if (meta.type == AssetType::Texture) {
    auto res = loadTexture(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::LuaScript) {
    auto res = loadLuaScript(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Animator) {
    auto res = loadAnimator(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::InputMap) {
    auto res = loadInputMap(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Audio) {
    auto res = loadAudio(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Font) {
    auto res = loadFont(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Scene) {
    auto res = loadScene(uuid);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Material) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadMaterialDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Mesh) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadMeshDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::SkinnedMesh) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadMeshDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Skeleton) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadSkeletonDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Animation) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadAnimationDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Prefab) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadPrefabDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Environment) {
    InputBinaryStream stream(path);
    AssetFileHeader header;
    stream.read(header);

    auto res = loadEnvironmentDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  return Result<bool>::Error("Unknown asset file: " + path.stem().string());
}

Result<Path> AssetCache::createAssetMeta(AssetType type, String name,
                                         Path path) {
  auto metaPath = path.replace_extension("assetmeta");
  OutputBinaryStream stream(path);

  if (!stream.good()) {
    return Result<Path>::Error("Cannot create meta file for asset: " +
                               path.stem().string());
  }

  stream.write(type);
  stream.write(name);

  return Result<Path>::Ok(metaPath);
}

Path AssetCache::getPathFromUuid(const Uuid &uuid) {
  return (mAssetsPath / uuid.toString()).replace_extension("asset");
}

} // namespace quoll
