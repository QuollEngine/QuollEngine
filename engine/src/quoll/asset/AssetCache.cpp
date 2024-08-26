#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

AssetCache::AssetCache(const Path &assetsPath, bool createDefaultObjects)
    : mAssetsPath(assetsPath) {
  if (createDefaultObjects) {
    mRegistry.createDefaultObjects();
  }
}

Result<void> AssetCache::preloadAssets(RenderStorage &renderStorage) {
  QUOLL_PROFILE_EVENT("AssetCache::preloadAssets");
  std::vector<String> warnings;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file() || entry.path().extension() == ".assetmeta") {
      continue;
    }

    auto res = loadAsset(entry.path());

    if (!res) {
      warnings.push_back(res.error());
    } else {
      warnings.insert(warnings.end(), res.warnings().begin(),
                      res.warnings().end());
    }
  }

  mRegistry.syncWithDevice(renderStorage);

  return {warnings};
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

Result<void> AssetCache::loadAsset(const Path &path) {
  auto uuid = Uuid(path.stem().string());
  auto meta = getAssetMeta(uuid);

  if (meta.type == AssetType::Texture) {
    auto res = load<TextureAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::LuaScript) {
    auto res = load<LuaScriptAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::Animator) {
    auto res = load<AnimatorAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::InputMap) {
    auto res = load<InputMapAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::Audio) {
    auto res = load<AudioAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::Font) {
    auto res = load<FontAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::Scene) {
    auto res = load<SceneAsset>(uuid);
    if (!res) {
      return res.error();
    }

    return {res.warnings()};
  }

  if (meta.type == AssetType::Material) {
    auto res = load<MaterialAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  if (meta.type == AssetType::Mesh) {
    auto res = load<MeshAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  if (meta.type == AssetType::SkinnedMesh) {
    auto res = load<MeshAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  if (meta.type == AssetType::Skeleton) {
    auto res = load<SkeletonAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  if (meta.type == AssetType::Animation) {
    auto res = load<AnimationAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  if (meta.type == AssetType::Prefab) {
    auto res = load<PrefabAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  if (meta.type == AssetType::Environment) {
    auto res = load<EnvironmentAsset>(uuid);

    if (!res) {
      return res.error();
    }
    return {res.warnings()};
  }

  return Error("Unknown asset file: " + path.stem().string());
}

Result<Path> AssetCache::createAssetMeta(AssetType type, String name,
                                         Path path) {
  auto metaPath = path.replace_extension("assetmeta");
  OutputBinaryStream stream(path);

  if (!stream.good()) {
    return Error("Cannot create meta file for asset: " + path.stem().string());
  }

  stream.write(type);
  stream.write(name);

  return metaPath;
}

Path AssetCache::getPathFromUuid(const Uuid &uuid) const {
  return (mAssetsPath / uuid.toString()).replace_extension("asset");
}

} // namespace quoll
