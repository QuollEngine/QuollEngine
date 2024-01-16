#pragma once

#include "quoll/core/Uuid.h"
#include "AssetData.h"

namespace quoll {

template <class THandle, class TData> class AssetMap {
public:
  using Handle = THandle;

public:
  THandle addAsset(const AssetData<TData> &data) {
    auto handle = getNewHandle();
    mAssets.insert_or_assign(handle, data);
    return handle;
  }

  void updateAsset(THandle handle, const AssetData<TData> &data) {
    QuollAssert(mAssets.find(handle) != mAssets.end(), "Asset does not exist");
    mAssets.at(handle) = data;
  }

  const AssetData<TData> &getAsset(THandle handle) const {
    return mAssets.at(handle);
  }

  AssetData<TData> &getAsset(THandle handle) { return mAssets.at(handle); }

  inline const std::unordered_map<THandle, AssetData<TData>> &
  getAssets() const {
    return mAssets;
  }

  inline THandle findHandleByUuid(const Uuid &uuid) const {
    for (auto &[handle, data] : mAssets) {
      if (data.uuid == uuid) {
        return handle;
      }
    }

    return THandle::Null;
  }

  inline std::unordered_map<THandle, AssetData<TData>> &getAssets() {
    return mAssets;
  }

  inline bool hasAsset(THandle handle) const {
    return mAssets.find(handle) != mAssets.end();
  }

  void deleteAsset(THandle handle) { mAssets.erase(handle); }

private:
  THandle getNewHandle() {
    THandle handle = mLastHandle;
    mLastHandle = THandle{static_cast<u32>(mLastHandle) + 1};
    return handle;
  }

private:
  std::unordered_map<THandle, AssetData<TData>> mAssets;
  THandle mLastHandle{1};
};

} // namespace quoll
