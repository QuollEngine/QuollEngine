#pragma once

#include "quoll/core/Uuid.h"
#include "AssetData.h"
#include "AssetHandle.h"

namespace quoll {

template <class TData> class AssetMap {
public:
  using Handle = AssetHandle<TData>;

public:
  Handle addAsset(const AssetData<TData> &data) {
    auto handle = getNewHandle();
    mAssets.insert_or_assign(handle, data);
    return handle;
  }

  void updateAsset(Handle handle, const AssetData<TData> &data) {
    QuollAssert(mAssets.find(handle) != mAssets.end(), "Asset does not exist");
    mAssets.at(handle) = data;
  }

  const AssetData<TData> &getAsset(Handle handle) const {
    return mAssets.at(handle);
  }

  AssetData<TData> &getAsset(Handle handle) { return mAssets.at(handle); }

  inline const std::unordered_map<Handle, AssetData<TData>> &getAssets() const {
    return mAssets;
  }

  inline Handle findHandleByUuid(const Uuid &uuid) const {
    for (auto &[handle, data] : mAssets) {
      if (data.uuid == uuid) {
        return handle;
      }
    }

    return Handle();
  }

  inline std::unordered_map<Handle, AssetData<TData>> &getAssets() {
    return mAssets;
  }

  inline bool hasAsset(Handle handle) const {
    return mAssets.find(handle) != mAssets.end();
  }

  void deleteAsset(Handle handle) { mAssets.erase(handle); }

private:
  Handle getNewHandle() { return Handle(mLastHandle++); }

private:
  std::unordered_map<Handle, AssetData<TData>> mAssets;
  AssetHandleType mLastHandle = 1;
};

} // namespace quoll
