#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"

#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

#include <uuid.h>

namespace liquid {

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
    return Result<AssetFileHeader>::Error(
        "Opened file is not a liquid asset: " + filePath.string());
  }

  if (header.type != assetType) {
    return Result<AssetFileHeader>::Error("Opened file is not a liquid " +
                                          getAssetTypeString(assetType) +
                                          " asset: " + filePath.string());
  }

  return Result<AssetFileHeader>::Ok(header);
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

AssetMeta AssetCache::getMetaFromUuid(const String &uuid) const {
  AssetMeta meta{};
  auto typePath = (mAssetsPath / uuid).replace_extension("assetmeta");
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

Result<bool> AssetCache::loadAsset(const Path &path, bool updateExisting) {
  uint32_t handle = 0;
  if (updateExisting) {
    const auto &asset = mRegistry.getAssetByUuid(path.stem().string());
    handle = asset.second;

    if (asset.first != AssetType::None && asset.first != AssetType::LuaScript &&
        asset.first != AssetType::Animator) {
      return Result<bool>::Error(
          "Can only reload Lua scripts and animators on watch");
    }
  }

  // Handle files that are not in liquid format
  auto meta = getMetaFromUuid(path.stem().string());

  if (meta.type == AssetType::Texture) {
    auto res = loadTextureFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::LuaScript) {
    auto res =
        loadLuaScriptFromFile(path, static_cast<LuaScriptAssetHandle>(handle));
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Animator) {
    auto res =
        loadAnimatorFromFile(path, static_cast<AnimatorAssetHandle>(handle));
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Audio) {
    auto res = loadAudioFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (meta.type == AssetType::Font) {
    auto res = loadFontFromFile(path);
    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }

    return Result<bool>::Ok(true, res.getWarnings());
  }

  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);

  if (header.magic != AssetFileHeader::MagicConstant) {
    return Result<bool>::Error("Not a liquid asset: " + path.stem().string());
  }

  if (header.type == AssetType::Material) {
    auto res = loadMaterialDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Mesh) {
    auto res = loadMeshDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::SkinnedMesh) {
    auto res = loadSkinnedMeshDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Skeleton) {
    auto res = loadSkeletonDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Animation) {
    auto res = loadAnimationDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Prefab) {
    auto res = loadPrefabDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (header.type == AssetType::Environment) {
    auto res = loadEnvironmentDataFromInputStream(stream, path, header);

    if (res.hasError()) {
      return Result<bool>::Error(res.getError());
    }
    return Result<bool>::Ok(true, res.getWarnings());
  }

  return Result<bool>::Error("Unknown asset file: " + path.stem().string());
}

Result<Path> AssetCache::createMetaFile(AssetType type, String name,
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

String liquid::AssetCache::generateUUID() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);

  uuids::uuid_random_generator gen{generator};
  auto id = gen();

  auto str = uuids::to_string(id);

  std::erase(str, '-');
  return str;
}

Path AssetCache::createAssetPath(const String &uuid) {
  auto stem = uuid.empty() ? generateUUID() : uuid;
  return (mAssetsPath / stem).replace_extension("asset").make_preferred();
}

Path AssetCache::getPathFromUuid(const String &uuid) {
  return (mAssetsPath / uuid).replace_extension("asset");
}

} // namespace liquid
