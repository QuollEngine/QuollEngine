#pragma once

#include "AssetData.h"
#include "AssetHandle.h"
#include "AssetMap.h"

namespace quoll {

template <class TAssetData> class AssetRef {
public:
  constexpr AssetRef() = default;
  constexpr explicit AssetRef(AssetMap<TAssetData> *map,
                              AssetHandle<TAssetData> handle)
      : mMap(map), mHandle(handle) {}

  constexpr operator bool() const { return mHandle; }
  constexpr TAssetData *operator->() { return &get(); }
  constexpr const TAssetData *operator->() const { return &get(); }

  constexpr TAssetData &get() { return mMap->getAsset(mHandle).data; }
  constexpr const TAssetData &get() const {
    return mMap->getAsset(mHandle).data;
  }
  constexpr AssetData<TAssetData> &meta() { return mMap->getAsset(mHandle); }
  constexpr const AssetData<TAssetData> &meta() const {
    return mMap->getAsset(mHandle);
  }

  constexpr const AssetHandle<TAssetData> &handle() const { return mHandle; }

private:
  AssetHandle<TAssetData> mHandle;
  AssetMap<TAssetData> *mMap = nullptr;
};

} // namespace quoll
