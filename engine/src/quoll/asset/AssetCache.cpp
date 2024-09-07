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
  meta.uuid = uuid;
  return meta;
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

std::vector<String> AssetCache::waitForAssetsToBeLoaded() {
  std::vector<String> warnings;

  for (auto &future : mQueuedLoads) {
    auto taskResult = future.get();
    if (taskResult) {
      auto loadResult = taskResult.data();
      if (loadResult && loadResult.hasWarnings()) {
        warnings.insert(warnings.end(), loadResult.warnings().begin(),
                        loadResult.warnings().end());
      } else {
        warnings.push_back(loadResult.error());
      }
    } else {
      warnings.push_back(taskResult.error());
    }
  }

  return std::move(warnings);
}

} // namespace quoll
