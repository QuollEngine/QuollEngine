#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

AssetCache::AssetCache(const Path &assetsPath, bool createDefaultObjects)
    : mAssetsPath(assetsPath), mDebugPanel(this) {
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

std::unordered_map<Uuid, Result<void>> AssetCache::waitForIdle() {
  // Note: We have two points of synchronization for assets.
  //   1. Current state of loads in the our loading waterfall using
  //       mutex, conditional variable, and atomic counter
  //   2. The futures that store the results of an async operation
  //
  // The current state of waterfall increments the atomic counter when an item
  // is being loaded and decrements it when loading is finished (error or
  // success) It also notifies the conditional variable.
  //
  // When waitForIdle is called (typically from main thread), the conditional
  // variable waits until the atomic counter is zero, meaning all assets up
  // until waitForIdle are loaded.
  //
  // At this point, we know that it is safe to release the lock (hence the
  // scope), and wait for the futures to be finished.

  {
    std::unique_lock<std::mutex> lock(mFuturesMutex);
    mLoadComplete.wait(lock, [this] { return mLoadCount == 0; });
  }

  std::unordered_map<Uuid, Result<void>> results;
  for (auto &[uuid, future] : mLoadFutures) {
    results.insert_or_assign(uuid, future.get());
  }

  mLoadFutures.clear();

  return results;
}

Result<void> AssetCache::waitForIdle(const Uuid &uuid) {
  {
    std::unique_lock<std::mutex> lock(mFuturesMutex);
    mLoadComplete.wait(lock, [this] { return mLoadCount == 0; });
  }

  auto it = mLoadFutures.find(uuid);
  if (it != mLoadFutures.end()) {
    auto res = it->second.get();
    mLoadFutures.erase(it);
    return res;
  }

  return Error("Uuid is not queued for loading");
}

} // namespace quoll
