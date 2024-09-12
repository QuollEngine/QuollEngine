#pragma once

#include "quoll/core/Uuid.h"
#include "AssetData.h"
#include "AssetHandle.h"
#include "AssetMeta.h"

namespace quoll {

template <class TData> class AssetMap {
public:
  using Handle = AssetHandle<TData>;

public:
  Handle allocate(const AssetMeta &meta) {
    std::lock_guard lock(mAllocateMutex);

    QuollAssert(!meta.uuid.isEmpty(), "Invalid uuid provided");
    auto it = mAssetUuids.find(meta.uuid);
    auto handle = it != mAssetUuids.end() ? it->second : getNewHandle();
    mAssetMetas.insert_or_assign(handle, meta);
    mAssetUuids.insert_or_assign(meta.uuid, handle);

    if (!mAssetReferenceCounts.contains(handle)) {
      mAssetReferenceCounts.insert_or_assign(handle, 0);
    }

    return handle;
  }

  void destroy(const Uuid &uuid) {
    QuollAssert(!mAssetUuids.contains(uuid), "Asset does not exist");

    auto handle = mAssetUuids.at(uuid);
    mAssetMetas.erase(handle);
    mAssetData.erase(handle);
    mAssetReferenceCounts.erase(handle);
    mAssetUuids.erase(uuid);
  }

  void store(Handle handle, const TData &data) {
    std::lock_guard lock(mStoreMutex);

    QuollAssert(mAssetMetas.contains(handle), "Asset does not exist");
    mAssetData.insert_or_assign(handle, data);
  }

  const TData &get(Handle handle) const {
    QuollAssert(mAssetMetas.contains(handle), "Asset does not exist");
    QuollAssert(mAssetData.contains(handle), "Asset has no data");

    return mAssetData.at(handle);
  }

  const AssetMeta &getMeta(Handle handle) const {
    QuollAssert(mAssetMetas.contains(handle), "Asset does not exist");
    return mAssetMetas.at(handle);
  }

  inline bool contains(Handle handle) const {
    return mAssetMetas.contains(handle);
  }

  inline bool hasData(Handle handle) const {
    return mAssetData.contains(handle);
  }

  void take(Handle handle) {
    QuollAssert(mAssetMetas.contains(handle), "Asset does not exist");

    auto it = mAssetReferenceCounts.find(handle);
    it->second = it->second + 1;
  }

  void release(Handle handle) {
    QuollAssert(mAssetMetas.contains(handle), "Asset does not exist");

    auto it = mAssetReferenceCounts.find(handle);
    QuollAssert(it->second > 0, "Asset cannot have reference count of zero");

    it->second = it->second - 1;
  }

  inline u32 getRefCount(Handle handle) const {
    return mAssetReferenceCounts.at(handle);
  }

  inline Handle findHandleByUuid(const Uuid &uuid) const {
    auto it = mAssetUuids.find(uuid);
    if (it == mAssetUuids.end()) {
      return Handle();
    }

    return it->second;
  }

  inline const std::unordered_map<Handle, AssetMeta> &getMetas() const {
    return mAssetMetas;
  }

  void clear() {
    mAssetUuids.clear();
    mAssetMetas.clear();
    mAssetData.clear();
    mAssetReferenceCounts.clear();
    mLastHandle = 1;
  }

private:
  Handle getNewHandle() { return Handle(mLastHandle++); }

private:
  AssetHandleType mLastHandle = 1;

  std::unordered_map<Uuid, Handle> mAssetUuids;
  std::unordered_map<Handle, AssetMeta> mAssetMetas;
  std::unordered_map<Handle, TData> mAssetData;
  std::unordered_map<Handle, u32> mAssetReferenceCounts;

  std::mutex mStoreMutex;
  std::mutex mAllocateMutex;
};

} // namespace quoll
